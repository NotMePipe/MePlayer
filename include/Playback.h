#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <SDL3_mixer/SDL_mixer.h>


class Playback {
public:
    explicit Playback(const char *filename);
    ~Playback();

    void Play() const;
    void Resume() const;
    void Pause() const;
    void Stop() const;

    void Restart() const;
    void JumpTo(int timestamp_in_seconds) const;

    [[nodiscard]] int GetTrackLength() const;
    [[nodiscard]] std::string GetFormattedTrackLength() const;
    [[nodiscard]] int GetPlaybackPosition() const;
    [[nodiscard]] std::string GetFormattedPlaybackPosition() const;
private:
    int dur_seconds = 0;
    std::string dur_formatted;

    MIX_Mixer *mixer;
    MIX_Audio *audio;
    MIX_Track *track;
};

#endif //PLAYBACK_H
