#ifndef Playback_H
#define Playback_H

#include <string>

#include <SDL3/SDL_audio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class Track {
public:
    explicit Track(const char *filename);
    ~Track();

    void Play() const;
    void Pause() const;
    void Restart();

    void Jump(int seconds);
    void Seek(long long timestamp) const;

    [[nodiscard]] int GetRawTrackLength() const;
    [[nodiscard]] std::string GetTrackLength() const;

    [[nodiscard]] int GetRawPlaybackPosition() const;
    [[nodiscard]] std::string GetPlaybackPosition() const;
private:
    int streamIndex = -1;

    int dur_seconds = 0;
    int track_pos_seconds = 0;

    SDL_AudioSpec spec{};

    SDL_AudioDeviceID device;
    SDL_AudioStream *stream;

    AVFormatContext *format_context = nullptr;
    AVCodecContext *codec_context;

    int FFmpeg_to_SDL() const; //NOLINT

    static AVSampleFormat Planar_to_Packed(AVSampleFormat fmt);
    static SDL_AudioFormat FFmpeg_to_SDL_Audio_Format(AVSampleFormat fmt);
};

#endif //Playback_H
