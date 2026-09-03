#ifndef MEPLAYER_UTILS_H
#define MEPLAYER_UTILS_H

#include <SDL3/SDL_stdinc.h>
#include <unordered_set>
#include <string>

const extern Uint32 TRACK_PLAY_EVENT;
const extern Uint32 TRACK_DESTROY_EVENT;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern float SCROLL_SCALE;

const extern std::unordered_set<std::string> COMPATIBLE_TYPES;

bool IsCompatibleType(const char *path);

#endif //MEPLAYER_UTILS_H
