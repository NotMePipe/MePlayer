#include "Utils.h"

#include <algorithm>
#include <SDL3/SDL_events.h>

const Uint32 TRACK_PLAY_EVENT = SDL_RegisterEvents(1);
const Uint32 TRACK_DESTROY_EVENT = SDL_RegisterEvents(1);

int SCREEN_WIDTH = 960;
int SCREEN_HEIGHT = 540;
float SCROLL_SCALE = 10.0f;

const std::unordered_set<std::string> COMPATIBLE_TYPES = {
    "mp3", "mp2", "aac", "m4a", "m4b", "flac", "wav", "wave", "aiff", "aif", "ogg", "oga", "opus", "wma", "ape", "wv",
    "tta", "mka"
};

bool IsCompatibleType(const char *path) {
    const std::string filename = path;

    const size_t lastSlash = filename.find_last_of("/\\");
    const size_t lastDot = filename.find_last_of('.');

    if (lastDot == std::string::npos) {
        return false;
    }

    if (lastSlash != std::string::npos && lastDot < lastSlash) {
        return false;
    }

    if (lastDot + 1 >= filename.size()) {
        return false;
    }

    std::string ext = filename.substr(lastDot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](const unsigned char c) { return std::tolower(c); });

    return COMPATIBLE_TYPES.count(ext) > 0;
}