#include "Events.h"

#include <SDL3/SDL_events.h>

const Uint32 TRACK_PLAY_EVENT = SDL_RegisterEvents(1);
const Uint32 TRACK_DESTROY_EVENT = SDL_RegisterEvents(1);