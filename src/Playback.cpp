#include <iostream>
#include <string>

#include "Playback.h"

Uint32 progressed_bytes = 0;

unsigned int BytesToSeconds(Uint32 total_bytes, int sample_rate, int bytes_per_sample, int num_channels);
std::string FormatTime(unsigned int seconds);
void GetDataCallback(void *userdata, SDL_AudioStream *audio_stream, int additional_amount, int total_amount);
void PutDataCallback(void *userdata, SDL_AudioStream *audio_stream, int additional_amount, int total_amount);

Playback::Playback(const char *filename) {
    if (!SDL_LoadWAV(filename, &audio_spec, &buffer, &buffer_length)) {
        std::cout << "Playback Error: Failed to load WAV: " << SDL_GetError() << std::endl;
        return;
    }

    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec);
    if (!device) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        SDL_free(buffer);

        return;
    }

    stream = SDL_CreateAudioStream(&audio_spec, &audio_spec);
    if (!stream) {
        std::cerr << "Failed to create audio stream: " << SDL_GetError() << std::endl;
        SDL_CloseAudioDevice(device);
        SDL_free(buffer);
        return;
    }

    if (!SDL_BindAudioStream(device, stream)) {
        std::cerr << "Failed to bind audio stream: " << SDL_GetError() << std::endl;
        SDL_DestroyAudioStream(stream);
        SDL_CloseAudioDevice(device);
        SDL_free(buffer);
        return;
    }

    if (!SDL_PutAudioStreamData(stream, buffer, static_cast<int>(buffer_length))) {
        SDL_DestroyAudioStream(stream);
        SDL_CloseAudioDevice(device);
        SDL_free(buffer);
        return;
    }

    Pause();

    CalculateTrackLength();

    SDL_SetAudioStreamGetCallback(stream, &GetDataCallback, &track_progress);
    SDL_SetAudioStreamPutCallback(stream, &PutDataCallback, nullptr);
}

Playback::~Playback() {
    SDL_DestroyAudioStream(stream);
    SDL_CloseAudioDevice(device);
    SDL_free(buffer);
}

void Playback::Pause() const {
    SDL_PauseAudioDevice(device);
}

void Playback::Resume() const {
    SDL_ResumeAudioDevice(device);
}

void Playback::Restart() const {
    Pause();
    SDL_ClearAudioStream(stream);

    if (!SDL_PutAudioStreamData(stream, buffer, static_cast<int>(buffer_length))) {
        SDL_DestroyAudioStream(stream);
        SDL_CloseAudioDevice(device);
        SDL_free(buffer);
        return;
    }
    Resume();
}

void Playback::CalculateTrackLength() {
    const unsigned int seconds = BytesToSeconds(buffer_length, audio_spec.freq, SDL_AUDIO_BYTESIZE(audio_spec.format), audio_spec.channels);

    dur_seconds = static_cast<int>(seconds);
    dur_formatted = FormatTime(seconds);
}

int Playback::GetTrackLength() const {
    return dur_seconds;
}

std::string Playback::GetFormattedTrackLength() const {
    return dur_formatted;
}

std::string Playback::GetTrackProgress() const {
    return track_progress;
}

Uint32 Playback::GetBufferLength() const {
    return buffer_length;
}

unsigned int BytesToSeconds(const Uint32 total_bytes, const int sample_rate, const int bytes_per_sample, const int num_channels) {
    const unsigned int bytesPerSecond = sample_rate * bytes_per_sample * num_channels;
    return total_bytes / bytesPerSecond;
}

std::string FormatTime(const unsigned int seconds) {
    const unsigned int min = seconds / 60;

    std::string s_seconds = std::to_string(seconds - (min * 60));
    if (s_seconds.length() == 1) {
        s_seconds = "0" + s_seconds;
    }

    return std::to_string(min) + ":" + s_seconds;
}

void GetDataCallback(void *userdata, SDL_AudioStream *audio_stream, int additional_amount, const int total_amount) {
    progressed_bytes += total_amount;

    SDL_AudioSpec spec;
    SDL_GetAudioStreamFormat(audio_stream, nullptr, &spec);

    auto *output = static_cast<std::string *>(userdata);
    *output = FormatTime(BytesToSeconds(progressed_bytes, spec.freq, SDL_AUDIO_BYTESIZE(spec.format), 1));
}

void PutDataCallback(void *userdata, SDL_AudioStream *audio_stream, int additional_amount, int total_amount) {
    progressed_bytes = 0;
}
