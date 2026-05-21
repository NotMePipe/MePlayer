#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "PlaybackQueue.h"
#include "Track.h"

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
        SDL_Quit();
        return -1;
    }

    bool quit = false;
    SDL_Event event;

    auto *queue = PlaybackQueue::GetPlaybackQueue();
    queue->Enqueue("Heartbeat.wav");
    queue->Enqueue("Love & Money.m4a");

    Track *playback;
    queue->Play(&playback, 0);

    bool debounce = false;

    while (!quit) {
        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE]) {
            if (!debounce) {
                debounce = true;
                if (playback->IsPaused()) {
                    playback->Play();
                } else {
                    playback->Pause();
                }
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE]) {
            if (!debounce) {
                debounce = true;
                playback->Restart();
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT]) {
            if (!debounce) {
                debounce = true;
                playback->Jump(5);
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LEFT]) {
            if (!debounce) {
                debounce = true;
                playback->Jump(-5);
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RETURN]) {
            if (!debounce) {
                debounce = true;
                playback->Jump(60);
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_BACKSPACE]) {
            if (!debounce) {
                debounce = true;
                if (queue->Repeat() < 0) {
                    queue->Repeat(static_cast<unsigned int>(queue->GetCurrentIndex()));
                } else {
                    queue->Repeat(false);
                }
            }
        } else if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_DELETE]) {
            if (!debounce) {
                debounce = true;
                if (queue->Repeat() > -2) {
                    queue->Repeat(true);
                } else {
                    queue->Repeat(false);
                }
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

        std::string track_progress = playback->GetPlaybackPosition();
        std::string playback_length = playback->GetTrackLength();

        SDL_SetWindowTitle(window, track_progress.append("/").append(playback_length).c_str());

        if (playback->TrackEnded()) {
            if (queue->Next(&playback) < 0) { // TODO Remove/reimplement temporary crash prevention
                quit = true;
            }
        }
    }

    PlaybackQueue::Close();

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
