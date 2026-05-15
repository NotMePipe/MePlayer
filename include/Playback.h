#ifndef Playback_H
#define Playback_H

#include <SDL3/SDL_audio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class Playback {
public:
    explicit Playback(const char *filename);
    ~Playback();

    void Resume() const;
    void Pause() const;
private:
    SDL_AudioDeviceID device;
    SDL_AudioStream *stream;

    AVFormatContext *format_context = nullptr;
    AVCodecContext *codec_context;
    SwrContext *swr = nullptr;
};

#endif //Playback_H
