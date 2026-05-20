#include <iostream>

extern "C" {
#include <libswresample/swresample.h>
}

#include "Track.h"

const SDL_AudioSpec *spec_ref; // I feel very strongly that this is stupid

int progressed_bytes = 0;

void GetDataCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);

Track::Track(const char *filename) {
    if (avformat_open_input(&format_context, filename, nullptr, nullptr) < 0) {
        std::cout << "failed to open\n";
        return;
    }

    streamIndex = -1;

    for (unsigned int i = 0; i < format_context->nb_streams; ++i) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            streamIndex = static_cast<int>(i);
            break;
        }
    }

    if (streamIndex == -1) {
        std::cout << "no audio stream found\n";
        return;
    }

    const AVCodecParameters *params = format_context->streams[streamIndex]->codecpar;

    const AVCodec *codec = avcodec_find_decoder(params->codec_id);

    codec_context = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(codec_context, params);

    if (avcodec_open2(codec_context, codec, nullptr) < 0) {
        std::cerr << "Could not open codec\n";
        return;
    }

    spec.format = FFmpeg_to_SDL_Audio_Format(codec_context->sample_fmt);
    spec.channels = codec_context->ch_layout.nb_channels;
    spec.freq = codec_context->sample_rate;

    device = SDL_OpenAudioDevice(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec
    );

    if (!device) {
        std::cout << SDL_GetError() << "\n";
        return;
    }

    stream = SDL_CreateAudioStream(
        &spec,
        &spec
    );

    if (!stream) {
        std::cerr << "SDL_CreateAudioStream failed: " << SDL_GetError() << "\n";
        return;
    }

    SDL_BindAudioStream(device, stream);

    SDL_PauseAudioDevice(device);

    dur_seconds = FFmpeg_to_SDL();

    SDL_SetAudioStreamGetCallback(stream, GetDataCallback, &track_pos_seconds);
}

Track::~Track() {
    SDL_UnbindAudioStream(stream);

    SDL_DestroyAudioStream(stream);

    SDL_CloseAudioDevice(device);

    avcodec_free_context(&codec_context);

    avformat_close_input(&format_context);
}

void Track::Pause() const {
    SDL_PauseAudioDevice(device);
}

void Track::Play() const {
    SDL_ResumeAudioDevice(device);
}

void Track::Restart() {
    Seek(0);
    track_pos_seconds = 0;
    progressed_bytes = 0;
}

void Track::Jump(int seconds) {
    if (track_pos_seconds + seconds <= 0) {
        Restart();
        return;
    }

    if (track_pos_seconds + seconds > dur_seconds) {
        seconds = dur_seconds - track_pos_seconds; // TODO Implement track ending
    }

    Seek(track_pos_seconds + seconds);
    track_pos_seconds += seconds;
    progressed_bytes += static_cast<int>(seconds * (spec.freq * SDL_AUDIO_BYTESIZE(spec.format) * spec.channels));
}

void Track::Seek(const long long timestamp) const {
    av_seek_frame(format_context, -1, timestamp * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD);
    SDL_ClearAudioStream(stream);
    FFmpeg_to_SDL(); // NOLINT
}

int Track::GetRawTrackLength() const {
    return dur_seconds;
}

std::string Track::GetTrackLength() const {
    const int min = dur_seconds / 60;
    const int sec = dur_seconds - (min * 60);

    if (sec < 10) {
        return std::to_string(min) + ":0" + std::to_string(sec);
    }

    return std::to_string(min) + ":" + std::to_string(sec);
}

int Track::GetRawPlaybackPosition() const {
    return track_pos_seconds;
}

std::string Track::GetPlaybackPosition() const {
    const int min = track_pos_seconds / 60;
    const int sec = track_pos_seconds - (min * 60);

    if (sec < 10) {
        return std::to_string(min) + ":0" + std::to_string(sec);
    }

    return std::to_string(min) + ":" + std::to_string(sec);
}

int Track::FFmpeg_to_SDL() const {
    SwrContext *swr = nullptr;

    const AVSampleFormat packed_format = Planar_to_Packed(codec_context->sample_fmt);

    swr_alloc_set_opts2(
        &swr,

        &codec_context->ch_layout,
        packed_format,
        codec_context->sample_rate,

        &codec_context->ch_layout,
        codec_context->sample_fmt,
        codec_context->sample_rate,

        0,
        nullptr
    );

    swr_init(swr);

    long long int total_bytes = 0;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(format_context, pkt) >= 0) {
        if (pkt->stream_index != streamIndex) {
            av_packet_unref(pkt);
            continue;
        }

        if (avcodec_send_packet(codec_context, pkt) < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (avcodec_receive_frame(codec_context, frame) == 0) {
            const int64_t outSamples = av_rescale_rnd(
                swr_get_delay(swr, codec_context->sample_rate) + frame->nb_samples,
                codec_context->sample_rate,
                codec_context->sample_rate,
                AV_ROUND_UP
            );

            uint8_t *outBuffer = nullptr;

            av_samples_alloc(
                &outBuffer,
                nullptr,
                spec.channels,
                static_cast<int>(outSamples),
                packed_format,
                0
            );

            const int samples = swr_convert(
                swr,
                &outBuffer,
                static_cast<int>(outSamples),
                frame->data,
                frame->nb_samples
            );

            const int bytes = av_samples_get_buffer_size(
                nullptr,
                spec.channels,
                samples,
                packed_format,
                1
            );

            total_bytes += bytes;

            if (!SDL_PutAudioStreamData(stream, outBuffer, bytes)) {
                std::cerr << "SDL_PutAudioStreamData failed: " << SDL_GetError() << "\n";
            }
            av_freep(&outBuffer);
            av_frame_unref(frame);
        }

        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);

    swr_free(&swr);

    spec_ref = &spec;

    return static_cast<int>(total_bytes / (spec.freq * SDL_AUDIO_BYTESIZE(spec.format) * spec.channels));
}

AVSampleFormat Track::Planar_to_Packed(const AVSampleFormat fmt) {
    switch (fmt)
    {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            return AV_SAMPLE_FMT_U8;

        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            return AV_SAMPLE_FMT_S16;

        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            return AV_SAMPLE_FMT_S32;

        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
            return AV_SAMPLE_FMT_FLT;
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
            return AV_SAMPLE_FMT_DBL;

        case AV_SAMPLE_FMT_S64:
        case AV_SAMPLE_FMT_S64P:
            return AV_SAMPLE_FMT_S64;

        case AV_SAMPLE_FMT_NONE:
            return AV_SAMPLE_FMT_NONE;
        case AV_SAMPLE_FMT_NB:
            return AV_SAMPLE_FMT_NB;
        default:
            return AV_SAMPLE_FMT_NONE;
    }
}

SDL_AudioFormat Track::FFmpeg_to_SDL_Audio_Format(const AVSampleFormat fmt)
{
    switch (fmt)
    {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
            return SDL_AUDIO_U8;

        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
            return SDL_AUDIO_S16;

        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
            return SDL_AUDIO_S32;

        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
            return SDL_AUDIO_F32;

        case AV_SAMPLE_FMT_S64:
        case AV_SAMPLE_FMT_S64P:
            return SDL_AUDIO_S32;

        case AV_SAMPLE_FMT_NONE:
        case AV_SAMPLE_FMT_NB:
        default:
            return SDL_AUDIO_UNKNOWN;
    }
}

void GetDataCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, const int total_amount) {
    progressed_bytes += total_amount;

    auto *output = static_cast<int *>(userdata);

    *output = static_cast<int>(progressed_bytes / (spec_ref->freq * 2 * SDL_AUDIO_BYTESIZE(spec_ref->format)));
}