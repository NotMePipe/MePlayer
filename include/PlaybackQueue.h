#ifndef PLAYBACK_QUEUE_H
#define PLAYBACK_QUEUE_H

#include <vector>

#include "Track.h"

class PlaybackQueue {
public:
    static PlaybackQueue *GetPlaybackQueue();
    static void Close();

    int Play(Track **track, unsigned int index);
    int Next(Track **track);

    int GetCurrentIndex() const;

    void Enqueue(const char *filename);

    int Repeat() const;
    void Repeat(bool toggle);
    void Repeat(unsigned int index);
private:
    static PlaybackQueue *instance;

    PlaybackQueue();
    ~PlaybackQueue();

    std::vector<const char *> queue;

    Track *currentTrack = nullptr;

    int currentIndex;
    int repeat = -1;
};


#endif //PLAYBACK_QUEUE_H
