#include "Album.h"

Album::Album(const char *name, const char *artist)
    : name(name), artist(artist)
{
    p_queue = PlaybackQueue::GetPlaybackQueue();
}

Album::~Album()
{
    track_list.clear();
    track_list.shrink_to_fit();
}

void Album::Insert(const char *filename, int trackno)
{
    if (track_list.size() == track_list.capacity())
    {
        track_list.reserve(track_list.capacity() * 2);
    }
    track_list.emplace_back(filename);

    // TODO Implement track number system
}

void Album::Enqueue() const {
    for (const auto &track : track_list)
    {
        p_queue->Enqueue(track.c_str());
    }
}

void Album::PlayTrack(Track **track, const int trackno) const {
    p_queue->Clear();
    Enqueue();
    p_queue->Play(track, trackno - 1);
}

const char *Album::GetAlbumName() const {
    return name;
}

const char *Album::GetAlbumArtist() const {
    return artist;
}

std::vector<std::string> Album::GetTrackList() const {
    return track_list;
}

