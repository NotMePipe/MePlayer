#include "LibraryHandler.h"

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

LibraryHandler::LibraryHandler() {
    prefPath = SDL_GetPrefPath("Not MePipe", "MePlayer");
}

LibraryHandler::~LibraryHandler() {
    SDL_free(prefPath);
}
