#ifndef LIBRARY_HANDLER_H
#define LIBRARY_HANDLER_H

#include <fstream>
#include <vector>
#include <string>

typedef struct TrackInfo {
    std::string path;
    std::string name;
} TrackInfo;

class LibraryHandler {
public:
    static LibraryHandler *GetLibraryHandler();
    static void Close();

    void Insert(const char *path, bool isDirectory);

    void GenerateInfo();

    std::vector<TrackInfo> GetAllInfo();

    void QueueTrack(const char *searchName) const;

private:
    static LibraryHandler *sInstance;

    char *prefPath;

    std::fstream library;
    std::vector<TrackInfo> info;

    LibraryHandler();
    ~LibraryHandler();
};

#endif //LIBRARY_HANDLER_H
