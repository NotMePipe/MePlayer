#include "Utils.h"

#include <SDL3/SDL_events.h>

const Uint32 TRACK_PLAY_EVENT = SDL_RegisterEvents(1);
const Uint32 TRACK_DESTROY_EVENT = SDL_RegisterEvents(1);

int SCREEN_WIDTH = 960;
int SCREEN_HEIGHT = 540;
float SCROLL_SCALE = 10.0f;