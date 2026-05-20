#include "PlaybackQueue.h"

PlaybackQueue *PlaybackQueue::instance;

PlaybackQueue *PlaybackQueue::GetPlaybackQueue() {
    if (instance == nullptr) {
        instance = new PlaybackQueue();
    }
    return instance;
}

void PlaybackQueue::Close() {
    if (instance == nullptr) {
        return;
    }
    delete instance;
    instance = nullptr;
}

PlaybackQueue::PlaybackQueue() {
    currentIndex = 0;
    queue = {};
}

PlaybackQueue::~PlaybackQueue() {
    queue.clear();
    queue.shrink_to_fit();
}

int PlaybackQueue::Play(Track **track, const unsigned int index) {
    if (currentTrack != nullptr) {
        delete currentTrack;
        currentTrack = nullptr;
        *track = nullptr;
    }

    if (index >= queue.size()) {
        return -1;
    }

    currentTrack = new Track(queue[index]);
    *track = currentTrack;
    currentTrack->Play();
    return 0;
}

int PlaybackQueue::Next(Track **track) {
    if (repeat == -1) {
        return Play(track, ++currentIndex);
    }

    if (repeat == -2) {
        if (++currentIndex >= queue.size()) {
            currentIndex = 0;
        }
        return Play(track, currentIndex);
    }

    return Play(track, repeat);
}

int PlaybackQueue::GetCurrentIndex() const {
    return currentIndex;
}

void PlaybackQueue::Enqueue(const char *filename) {
    if (queue.size() == queue.capacity()) {
        queue.reserve(queue.capacity() * 2);
    }
    queue.push_back(filename);
}

int PlaybackQueue::Repeat() const {
    return repeat;
}

void PlaybackQueue::Repeat(const bool toggle) {
    if (toggle) {
        repeat = -2;
    } else {
        repeat = -1;
    }
}

void PlaybackQueue::Repeat(const unsigned int index) {
    if (index < queue.size()) {
        repeat = index;
    }
}