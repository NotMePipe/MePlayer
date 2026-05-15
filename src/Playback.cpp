#include <iostream>

#include "Playback.h"

Playback::Playback(const char *filename) {
    if (avformat_open_input(&format_context, filename, nullptr, nullptr) < 0) {
        std::cout << "failed to open\n";
        return;
    }

    int audioStream = -1;

    for (unsigned int i = 0; i < format_context->nb_streams; ++i) {
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = static_cast<int>(i);
            break;
        }
    }

    if (audioStream == -1) {
        std::cout << "no audio stream found\n";
        return;
    }

    const AVCodecParameters *params = format_context->streams[audioStream]->codecpar;

    const AVCodec *codec = avcodec_find_decoder(params->codec_id);

    codec_context = avcodec_alloc_context3(codec);

    avcodec_parameters_to_context(codec_context, params);

    if (avcodec_open2(codec_context, codec, nullptr) < 0) {
        std::cerr << "Could not open codec\n";
        return;
    }

    int outRate = 44100;
    AVSampleFormat outFmt = AV_SAMPLE_FMT_S16;

    AVChannelLayout outLayout;
    av_channel_layout_default(&outLayout, 2);

    swr = nullptr;

    swr_alloc_set_opts2(
        &swr,

        &outLayout,
        outFmt,
        outRate,

        &codec_context->ch_layout,
        codec_context->sample_fmt,
        codec_context->sample_rate,

        0,
        nullptr
    );

    swr_init(swr);

    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;
    spec.freq = outRate;

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

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(format_context, pkt) >= 0) {
        if (pkt->stream_index != audioStream) {
            av_packet_unref(pkt);
            continue;
        }

        if (avcodec_send_packet(codec_context, pkt) < 0) {
            av_packet_unref(pkt);
            continue;
        }

        while (avcodec_receive_frame(codec_context, frame) == 0) {
            // Number of output samples
            const int64_t outSamples = av_rescale_rnd(
                swr_get_delay(swr, codec_context->sample_rate) + frame->nb_samples,
                outRate,
                codec_context->sample_rate,
                AV_ROUND_UP
            );

            uint8_t *outBuffer;
            int outLineSize = 0;

            av_samples_alloc(
                &outBuffer,
                &outLineSize,
                2,
                static_cast<int>(outSamples),
                AV_SAMPLE_FMT_S16,
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
                2,
                samples,
                AV_SAMPLE_FMT_S16,
                1
            );

            // Push PCM to SDL3 stream
            if (!SDL_PutAudioStreamData(stream, outBuffer, bytes)) {
                std::cerr << "SDL_PutAudioStreamData failed: " << SDL_GetError() << "\n";
            }
            av_freep(&outBuffer);
        }

        av_packet_unref(pkt);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
}

Playback::~Playback() {
    SDL_DestroyAudioStream(stream);

    SDL_CloseAudioDevice(device);

    swr_free(&swr);

    avcodec_free_context(&codec_context);

    avformat_close_input(&format_context);
}

void Playback::Pause() const {
    SDL_PauseAudioDevice(device);
}

void Playback::Resume() const {
    SDL_ResumeAudioDevice(device);
}
