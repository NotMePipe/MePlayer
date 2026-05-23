#include "LibraryHandler.h"

#include <cstring>

#include "SDL3/SDL_filesystem.h"

LibraryHandler *LibraryHandler::sInstance;

LibraryHandler *LibraryHandler::GetLibraryHandler() {
    if (sInstance == nullptr) {
        sInstance = new LibraryHandler();
    }
    return sInstance;
}

void LibraryHandler::Close() {
    if (sInstance == nullptr) {
        return;
    }
    delete sInstance;
    sInstance = nullptr;
}

// TODO Currently library resets every time app starts. I do not think this should happen
LibraryHandler::LibraryHandler() {
    prefPath = SDL_GetPrefPath("Not MePipe", "MePlayer");

    library.open(strcat(prefPath, "library.dat"), std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
}

LibraryHandler::~LibraryHandler() {
    SDL_free(prefPath);

    library.close();
}

void LibraryHandler::Insert(const char *path, const bool isDirectory) {
    if (isDirectory) {
        // library << "dir: " << path << "\n";
    } else {
        library << path << "\n";
    }
}
