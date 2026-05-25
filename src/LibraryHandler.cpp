#include "LibraryHandler.h"

#include <cstring>

#include <SDL3/SDL_filesystem.h>

#include "PlaybackQueue.h"

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

void LibraryHandler::CreateButtons(SDL_Renderer *renderer) {
    library.clear();
    library.seekg(0, std::ios::beg);
    std::string line;
    while (std::getline(library, line)) {
        auto *b = new TextButton(25, buttons.size() * 50, 200, 50, 5, "Roboto.ttf", 50);

        TrackInfo i{line, line.substr(line.find_last_of("/\\") + 1)};
        info.push_back(i);

        b->SetText(renderer, i.name.c_str());
        buttons.push_back(b);
    }
}

void LibraryHandler::Render(SDL_Renderer *renderer) const {
    for (const auto button : buttons) {
        button->Render(renderer);
    }
}

std::vector<Button *> LibraryHandler::GetButtons() {
    return buttons;
}

void LibraryHandler::QueueTrack(const char *searchName) const {
    for (const auto &[path, name] : info) {
        if (name == searchName) {
            PlaybackQueue::GetPlaybackQueue()->Enqueue(path.c_str());
        }
    }
}
