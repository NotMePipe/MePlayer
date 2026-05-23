#ifndef PLAYBACK_QUEUE_H
#define PLAYBACK_QUEUE_H

#include <string>
#include <vector>

#include "Track.h"

class PlaybackQueue {
public:
    static PlaybackQueue *GetPlaybackQueue();
    static void Close();

    int Play(Track **track, unsigned int index);
    int Next(Track **track);

    [[nodiscard]] int GetCurrentIndex() const;

    void Enqueue(const char *filename);

    [[nodiscard]] int Repeat() const;
    void Repeat(bool toggle);
    void Repeat(unsigned int index);
private:
    static PlaybackQueue *sInstance;

    PlaybackQueue();
    ~PlaybackQueue();

    std::vector<std::string> queue;

    Track *currentTrack = nullptr;

    int currentIndex;
    int repeat = -1;
};


#endif //PLAYBACK_QUEUE_H
