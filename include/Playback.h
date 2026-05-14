#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "SDL3/SDL_audio.h"

class Playback {
public:
    explicit Playback(const char *filename);
    ~Playback();

    void Resume() const;
    void Pause() const;

    void Restart() const;

    [[nodiscard]] int GetTrackLength() const;
    [[nodiscard]] std::string GetFormattedTrackLength() const;
    [[nodiscard]] std::string GetTrackProgress() const;

    [[nodiscard]] Uint32 GetBufferLength() const;
private:
    SDL_AudioSpec audio_spec{};
    Uint8 *buffer = nullptr;
    Uint32 buffer_length = 0;

    SDL_AudioDeviceID device;
    SDL_AudioStream *stream;

    int dur_seconds = 0;
    std::string dur_formatted;
    std::string track_progress;

    void CalculateTrackLength();
};

#endif //PLAYBACK_H
