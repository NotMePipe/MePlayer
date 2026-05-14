#include <iostream>
#include <string>

#include "Playback.h"

std::string FormatTime(unsigned int seconds);

Playback::Playback(const char *filename) {
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer) {
        std::cerr << "Failed to create MIX device: " << SDL_GetError() << std::endl;
        return;
    }

    audio = MIX_LoadAudio(mixer, filename, false);
    if (!audio) {
        std::cerr << "Failed to load audio: " << SDL_GetError() << std::endl;
        MIX_DestroyMixer(mixer);
        return;
    }

    track = MIX_CreateTrack(mixer);
    if (!track) {
        std::cerr << "Failed to create track: " << SDL_GetError() << std::endl;
        MIX_DestroyAudio(audio);
        MIX_DestroyMixer(mixer);
        return;
    }

    if (!MIX_SetTrackAudio(track, audio)) {
        std::cerr << "Failed to set track audio: " << SDL_GetError() << std::endl;
        MIX_DestroyTrack(track);
        MIX_DestroyAudio(audio);
        MIX_DestroyMixer(mixer);
        return;
    }

    const Sint64 sample_frames = MIX_GetAudioDuration(audio);

    if (sample_frames < 0) {
        std::cerr << "Unable to determine track length" << std::endl;
        return;
    }

    const Sint64 ms = MIX_AudioFramesToMS(audio, sample_frames);

    dur_seconds = static_cast<int>(ms / 1000);
    dur_formatted = FormatTime(dur_seconds);
}

Playback::~Playback() {
    MIX_DestroyAudio(audio);
    MIX_DestroyTrack(track);
    MIX_DestroyMixer(mixer);
}

void Playback::Play() const {
    MIX_PlayTrack(track, 0);
}

void Playback::Pause() const {
    MIX_PauseTrack(track);
}

void Playback::Resume() const {
    MIX_ResumeTrack(track);
}

void Playback::Stop() const {
    MIX_StopTrack(track, 0);
}

void Playback::Restart() const {
    MIX_StopTrack(track, 0);
    MIX_PlayTrack(track, 0);
}

void Playback::JumpTo(int timestamp_in_seconds) const {
    if (timestamp_in_seconds < 0) {
        timestamp_in_seconds = 0;
    }

    if (timestamp_in_seconds > dur_seconds) {
        timestamp_in_seconds = dur_seconds;
    }

    if (!MIX_SetTrackPlaybackPosition(track, MIX_TrackMSToFrames(track, timestamp_in_seconds * 1000))) {
        std::cerr << "Failed to set track playback position: " << SDL_GetError() << std::endl;
    }
}

int Playback::GetTrackLength() const {
    return dur_seconds;
}

std::string Playback::GetFormattedTrackLength() const {
    return dur_formatted;
}

int Playback::GetPlaybackPosition() const {
    const Sint64 sample_frames = MIX_GetTrackPlaybackPosition(track);

    if (sample_frames < 0) {
        std::cerr << "Unable to determine track length" << std::endl;
        return -1;
    }

    const Sint64 ms = MIX_TrackFramesToMS(track, sample_frames);

    return static_cast<int>(ms / 1000);
}

std::string Playback::GetFormattedPlaybackPosition() const {
    return FormatTime(GetPlaybackPosition());
}

std::string FormatTime(const unsigned int seconds) {
    const unsigned int min = seconds / 60;

    std::string s_seconds = std::to_string(seconds - (min * 60));
    if (s_seconds.length() == 1) {
        s_seconds = "0" + s_seconds;
    }

    return std::to_string(min) + ":" + s_seconds;
}