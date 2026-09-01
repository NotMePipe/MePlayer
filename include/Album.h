#ifndef ALBUM_H
#define ALBUM_H

#include <vector>
#include <string>

#include "PlaybackQueue.h"

class Album {
public:
    Album(const char *name, const char *artist);
    ~Album();

    void Insert(const char *filename, int trackno = -1);

    void Enqueue() const;

    void PlayTrack(Track **track, int trackno) const;

    [[nodiscard]] const char *GetAlbumName() const;
    [[nodiscard]] const char *GetAlbumArtist() const;
    [[nodiscard]] std::vector<std::string> GetTrackList() const;

private:
    const char *name;
    const char *artist;

    std::vector<std::string> track_list;

    PlaybackQueue *p_queue;
};

#endif //ALBUM_H
