#ifndef Playback_H
#define Playback_H

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class Track {
public:
    explicit Track(const char *filename);
    ~Track();

    void Play();
    void Pause();
    void Restart();

    void Jump(int seconds);
    void Seek(long long timestamp) const;

    [[nodiscard]] int GetRawTrackLength() const;
    [[nodiscard]] const char *GetTrackLength() const;

    [[nodiscard]] int GetRawPlaybackPosition() const;
    [[nodiscard]] const char *GetPlaybackPosition() const;

    [[nodiscard]] bool IsPaused() const;
    [[nodiscard]] bool TrackEnded() const;

    [[nodiscard]] const char *Artist() const;
    [[nodiscard]] const char *Title() const;
    [[nodiscard]] const char *Album() const;
    [[nodiscard]] const char *AlbumArtist() const;
    [[nodiscard]] const char *Genre() const;

    [[nodiscard]] SDL_Surface *CoverArt() const;
private:
    int streamIndex = -1;

    int dur_seconds = 0;
    int track_pos_seconds = 0;

    bool paused;

    SDL_AudioSpec spec{};

    SDL_AudioDeviceID device;
    SDL_AudioStream *stream;

    AVFormatContext *format_context = nullptr;
    AVCodecContext *codec_context;

    SDL_Surface *coverArt = nullptr;

    int FFmpeg_to_SDL() const; //NOLINT

    bool LoadCoverArt();
};

#endif //Playback_H
