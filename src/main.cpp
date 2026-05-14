#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Playback.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("MePlayer", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window)
    {
        std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        return -1;
    }

    bool quit = false;
    SDL_Event event;

    const auto *playback = new Playback("./Heartbeat.wav");

    const std::string playback_length = playback->GetFormattedTrackLength();

    bool paused = true;
    bool debounce = false;

    while (!quit) {
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE]) {
            if (!debounce) {
                debounce = true;
                if (paused) {
                    paused = false;
                    playback->Resume();
                } else {
                    paused = true;
                    playback->Pause();
                }
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE]) {
            if (!debounce) {
                debounce = true;
                playback->Restart();
            }
        } else {
            debounce = false;
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                default: ;
            }
        }

        std::string track_progress = playback->GetTrackProgress();

        SDL_SetWindowTitle(window, track_progress.append("/").append(playback_length).c_str());
    }

    delete playback;

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
