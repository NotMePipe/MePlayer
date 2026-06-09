#define NEW_MAIN

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Frame.h"
#include "LibraryHandler.h"
#include "PlaybackQueue.h"
#include "ScrollingFrame.h"
#include "Track.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define SCROLL_SCALE 10

void ScanLibrary(LibraryHandler **lib, const char *path);
void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, unsigned int index);

int main(int argc, char* argv[]) {
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "MePlayer");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, VERSION);
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, "not.mepipe.meplayer");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, "Not MePipe");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, "https://github.com/NotMePipe/MePlayer");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "mediaplayer");

#ifdef NDEBUG
    av_log_set_level(AV_LOG_QUIET);
#endif

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("MePlayer", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    auto *playback_queue = PlaybackQueue::GetPlaybackQueue();

    auto *library_handler = LibraryHandler::GetLibraryHandler();
    ScanLibrary(&library_handler, "Music Folder");
    library_handler->GenerateInfo();

    // TODO Replace (ALL) constant sizing with dynamic scaling
    auto *songSelect = new ScrollingFrame(0, 25, 697.5, 511.875, 1);
    for (const auto &[path, name] : library_handler->GetAllInfo()) {
        // TODO Replace 232.5 and 3 (and/or dynamic equivalents) with user-specified values
        // TODO Find better height than 50
        auto *b = songSelect->Add(static_cast<float>(232.5 * (songSelect->NumChildren() % 3)), static_cast<float>(50  * static_cast<int>(songSelect->NumChildren() / 3)), 232.5, 50, 5, "Roboto.ttf", 50);
        b->SetText(renderer, name.c_str());
    }

    auto *queueFrame = new ScrollingFrame(697.5, 25, 262.5, 511.875, 2);
    playback_queue->SetQueueAddCallback(renderer, queueFrame, AddQueueButton);

    Track *playback = nullptr;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    songSelect->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    queueFrame->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    break;
                default:
                    songSelect->HandleEvent(event);
                    queueFrame->HandleEvent(event);
            }
        }

        if (playback != nullptr) {
            if (playback->IsPaused()) { // TODO Remove constant autoplay
                playback->Play();
            }

            if (playback->TrackEnded()) {
                if (playback_queue->Next(&playback) < 0) {
                    playback->Pause();
                }
            }
        } else {
            playback_queue->Next(&playback);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        songSelect->Render(renderer);
        queueFrame->Render(renderer);

        SDL_RenderPresent(renderer);
    }

    delete songSelect;
    delete queueFrame;

    PlaybackQueue::Close();
    LibraryHandler::Close();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}

void ScanLibrary(LibraryHandler **lib, const char *path) {
    SDL_PathInfo info;

    if (!SDL_GetPathInfo(path, &info)) {
        std::cerr << "Error in GetPathInfo: " << SDL_GetError() << " : " << path << "\n";
        return;
    }

    if (info.type == SDL_PATHTYPE_FILE) {
        (*lib)->Insert(path, false);
        return;
    }

    if (info.type == SDL_PATHTYPE_DIRECTORY) {
        int count = 0;

        char **entries = SDL_GlobDirectory(path, nullptr, 0, &count);

        if (!entries) {
            printf("Error: %s\n", SDL_GetError());
            return;
        }

        (*lib)->Insert(path, true);

        for (int i = 0; i < count; i++) {
            ScanLibrary(lib, std::string(path).append("/").append(entries[i]).c_str());
        }

        SDL_free(entries);
    }
}

void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, const unsigned int index) {
    auto *b = frame->Add(0, static_cast<float>(50 * frame->NumChildren()), frame->GetRect().w, 50, 5, "Roboto.ttf", 50);
    b->SetText(renderer, trackName);
    b->AssignIndex(index);
}