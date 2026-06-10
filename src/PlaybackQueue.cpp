#include "PlaybackQueue.h"

PlaybackQueue *PlaybackQueue::sInstance;

PlaybackQueue *PlaybackQueue::GetPlaybackQueue() {
    if (sInstance == nullptr) {
        sInstance = new PlaybackQueue();
    }
    return sInstance;
}

void PlaybackQueue::Close() {
    if (sInstance == nullptr) {
        return;
    }
    delete sInstance;
    sInstance = nullptr;
}

PlaybackQueue::PlaybackQueue() {
    currentIndex = -1;
    queue = {};
}

PlaybackQueue::~PlaybackQueue() {
    Clear();
}

int PlaybackQueue::Play(const int index) {
    if (currentTrack == nullptr) {
        return -2;
    }

    return Play(&currentTrack, index);
}

int PlaybackQueue::Play(Track **track, const int index) {
    if (index >= queue.size()) {
        return -1;
    }

    currentIndex = index;

    if (currentTrack != nullptr) {
        delete currentTrack;
        currentTrack = nullptr;
        *track = nullptr;
    }

    currentTrack = new Track(queue[index].c_str());
    *track = currentTrack;
    currentTrack->Play();
    return 0;
}

int PlaybackQueue::Next(Track **track) {
    if (repeat == -1) {
        if (Play(track, ++currentIndex) < 0) {
            --currentIndex;
            return -1;
        }
        return 0;
    }

    if (repeat == -2) {
        if (++currentIndex >= queue.size()) {
            currentIndex = 0;
        }
        return Play(track, currentIndex);
    }

    (*track)->Restart();
    return 0;
}

int PlaybackQueue::GetCurrentIndex() const {
    return currentIndex;
}

void PlaybackQueue::Enqueue(const char *filename) {
    if (queue.size() == queue.capacity()) {
        queue.reserve(queue.capacity() * 2);
    }
    queue.emplace_back(filename);

    if (addCallback != nullptr) {
        const std::string name = filename;
        addCallback(rend, f, name.substr(name.find_last_of("/\\") + 1).c_str(), static_cast<int>(queue.size()) - 1);
    }
}

void PlaybackQueue::Clear() {
    if (currentTrack != nullptr) {
        delete currentTrack;
        currentTrack = nullptr;
    }

    queue.clear();
    queue.shrink_to_fit();
    currentIndex = -1;
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
        repeat = static_cast<int>(index);
    }
}

void PlaybackQueue::SetQueueAddCallback(SDL_Renderer *renderer, Frame *frame, void (*callback)(SDL_Renderer *, Frame *, const char *, int)) {
    addCallback = callback;
    rend = renderer;
    f = frame;
}