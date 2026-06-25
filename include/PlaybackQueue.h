#ifndef PLAYBACK_QUEUE_H
#define PLAYBACK_QUEUE_H

#include <string>
#include <vector>

#include "Frame.h"
#include "Track.h"

class PlaybackQueue {
public:
    static PlaybackQueue *GetPlaybackQueue();
    static void Close();

    int Play(Track **track, int index);
    int Next(Track **track);

    [[nodiscard]] int GetCurrentIndex() const;

    void Enqueue(const char *filename);
    void Remove(int index);
    void Clear();

    [[nodiscard]] int Repeat() const;
    void Repeat(bool toggle);
    void Repeat(unsigned int index);

    void SetQueueAddCallback(SDL_Renderer *renderer, Frame *frame, void (*callback)(SDL_Renderer *, Frame *, const char *, int));
private:
    static PlaybackQueue *sInstance;

    PlaybackQueue();
    ~PlaybackQueue();

    std::vector<std::string> queue;

    Track *currentTrack = nullptr;

    int currentIndex;
    int repeat = -1;

    void (*addCallback)(SDL_Renderer *, Frame *, const char *, int) = nullptr;
    SDL_Renderer *rend = nullptr;
    Frame *f = nullptr;
};

#endif //PLAYBACK_QUEUE_H
