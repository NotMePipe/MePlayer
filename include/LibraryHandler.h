#ifndef LIBRARY_HANDLER_H
#define LIBRARY_HANDLER_H

#include <fstream>
#include <vector>
#include <string>

#include "TextButton.h"

typedef struct TrackInfo {
    std::string path;
    std::string name;
} TrackInfo;

class LibraryHandler {
public:
    static LibraryHandler *GetLibraryHandler();
    static void Close();

    void Insert(const char *path, bool isDirectory);

    void CreateButtons(SDL_Renderer *renderer); // TODO This is a temporary function
    void Render(SDL_Renderer *renderer) const; // TODO This is a temporary function
    std::vector<Button *> GetButtons(); // TODO This is a temporary function
    void QueueTrack(const char *searchName) const; // TODO This is a temporary function

private:
    static LibraryHandler *sInstance;

    char *prefPath;

    std::fstream library;
    std::vector<Button *> buttons;
    std::vector<TrackInfo> info;

    LibraryHandler();
    ~LibraryHandler();
};

#endif //LIBRARY_HANDLER_H
