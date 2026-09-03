#include "LibraryHandler.h"

#include <cstring>
#include <iostream>

#include <SDL3/SDL_filesystem.h>

#include "PlaybackQueue.h"
#include "Utils.h"

LibraryHandler *LibraryHandler::sInstance;

LibraryHandler *LibraryHandler::GetLibraryHandler()
{
    if (sInstance == nullptr)
    {
        sInstance = new LibraryHandler();
    }
    return sInstance;
}

void LibraryHandler::Close()
{
    if (sInstance == nullptr)
    {
        return;
    }
    delete sInstance;
    sInstance = nullptr;
}

// TODO Currently library resets every time app starts. I do not think this should happen
LibraryHandler::LibraryHandler()
{
    prefPath = SDL_GetPrefPath("Not MePipe", "MePlayer");

    library.open(strcat(prefPath, "library.dat"), std::ios::binary | std::ios::in | std::ios::out);
}

LibraryHandler::~LibraryHandler()
{
    SDL_free(prefPath);

    library.close();
}

ScanContext *LibraryHandler::GetContext()
{
    return &ctx;
}

void LibraryHandler::QueueTrack(const int index) const
{
    PlaybackQueue::GetPlaybackQueue()->Enqueue(ctx.files[index].c_str());
}

SDL_EnumerationResult scanCallback(void *userdata, const char *dirname, const char *filename) {
    auto *ctx = static_cast<ScanContext *>(userdata);
    std::string fullPath = std::string(dirname) + filename;

    SDL_PathInfo info;
    if (SDL_GetPathInfo(fullPath.c_str(), &info)) {
        if (info.type == SDL_PATHTYPE_DIRECTORY) {
            std::string subDir = fullPath + "/";
            SDL_EnumerateDirectory(subDir.c_str(), scanCallback, userdata);
        }
        else if (info.type == SDL_PATHTYPE_FILE) {
            if (IsCompatibleType(fullPath.c_str())) {
                const size_t trackName = fullPath.find_last_of("/\\") + 1;
                const size_t fileExtension = fullPath.rfind('.');

                ctx->titles.push_back(fullPath.substr(trackName, fileExtension - trackName));
                ctx->files.push_back(std::move(fullPath));

                ++ctx->count;
            }
        }
    }
    return SDL_ENUM_CONTINUE;
}

std::vector<std::string> LibraryHandler::ScanDirectory(const char *root)
{
    std::string dir = root;
    if (dir.empty() || dir.back() != '/') {
        dir += '/';
    }

    SDL_EnumerateDirectory(dir.c_str(), scanCallback, &ctx);

    return ctx.files;
}

void LibraryHandler::ScanLibrary() {
    // TODO Fetch list of directories
    ScanDirectory("Music Folder");

    if (!CheckCount()) {
        if (library.is_open()) {
            library.close();
        }
        library.clear();

        library.open(prefPath, std::ios::in | std::ios::out | std::ios::trunc);

        library << ctx.count << '\n';

        for (const auto& file : ctx.files) {
            library << file << '\n';
        }

        library.flush();
    }
}

bool LibraryHandler::CheckCount() {
    library.clear();

    library.seekg(0, std::ios::beg);

    int number = 0;
    if (!(library >> number)) {
        number = 0;
        library.clear();
    }

    return ctx.count == number;
}