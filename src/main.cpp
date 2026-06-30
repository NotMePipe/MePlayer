#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Events.h"
#include "LibraryHandler.h"
#include "PlaybackQueue.h"
#include "ScrollingFrame.h"
#include "Track.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define SCROLL_SCALE 10

void ScanLibrary(LibraryHandler **lib, const char *path);
void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, int index);
void AddToQueue(void *info);
void PlayInQueue(void *info);
void RemoveFromQueue(void *info);
// TODO These need overhauled
void PlayPause(void *info);
void RR(void *info);
void FF(void *info);

Track *playback;
// TODO Temporary playback handlers
bool skip = false;

// TODO Replace (ALL) constant sizing and positioning with dynamic scaling
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

    auto *topBar = new Button(0, 0, 960, 30, -1);
    topBar->SetButtonColor(255, 255, 255, 255);

    auto *songSelect = new ScrollingFrame(0, 30, 750, 465);
    for (const auto &[path, name] : library_handler->GetAllInfo()) {
        // TODO Find better height than 50
        const float width = songSelect->GetRect().w / 3;
        auto *b = songSelect->Add(width * static_cast<float>(songSelect->NumChildren() % 3), static_cast<float>(50  * static_cast<int>(songSelect->NumChildren() / 3)), width, 50, 5, "Roboto.ttf", 50);
        b->SetText(renderer, name.c_str());
        b->SetOnClickEvent(AddToQueue, nullptr); // TODO Determine if nullptr
    }

    auto *queueFrame = new ScrollingFrame(750, 30, 210, 465);
    playback_queue->SetQueueAddCallback(renderer, queueFrame, AddQueueButton);

    auto *playbackControls = new Frame(175, 465, 785, 75);
    playbackControls->SetColor(255, 0, 0, 255);
    // TODO These should all become image buttons
    {
        auto *playPause = playbackControls->Add(227, 12, 157, 50, 5, "Roboto.ttf", 50);
        playPause->SetText(renderer, "Play/Pause");
        playPause->SetOnClickEvent(PlayPause, nullptr); // TODO Determine if nullptr

        auto *rr = playbackControls->Add(50, 12, 157, 50, 5, "Roboto.ttf", 50);
        rr->SetText(renderer, "RR");
        rr->SetOnClickEvent(RR, nullptr); // TODO Determine if nullptr

        auto *ff = playbackControls->Add(404, 12, 157, 50, 5, "Roboto.ttf", 50);
        ff->SetText(renderer, "FF");
        ff->SetOnClickEvent(FF, nullptr);  // TODO Determine if nullptr
    }

    playback = nullptr;
    bool quit = false;
    SDL_Event event;
    SDL_Texture *trackCoverArt = nullptr;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    songSelect->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    queueFrame->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    break;
                case SDL_EVENT_KEY_DOWN: // TODO Remove when parenting is added
                    if (event.key.scancode == SDL_SCANCODE_DELETE) {
                        playback_queue->Remove(playback_queue->GetCurrentIndex());
                        playback_queue->Next(&playback);
                        queueFrame->Remove(0);
                    }
                    break;
                default:
                    if (event.type == TRACK_PLAY_EVENT) {
                        trackCoverArt = SDL_CreateTextureFromSurface(renderer, playback->CoverArt());
                    } else if (event.type == TRACK_DESTROY_EVENT) {
                        SDL_DestroyTexture(trackCoverArt);
                        trackCoverArt = nullptr;
                    } else {
                        songSelect->HandleEvent(event);
                        queueFrame->HandleEvent(event);
                        playbackControls->HandleEvent(event);
                    }
            }
        }

        if (playback != nullptr) {
            if (playback->TrackEnded() || skip) {
                skip = false;
                playback_queue->Next(&playback);
            }
        } else {
            if (playback_queue->Next(&playback) == 0) {
                playback->Play();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        topBar->Render(renderer);
        songSelect->Render(renderer);
        queueFrame->Render(renderer);

        if (trackCoverArt != nullptr) {
            SDL_FRect dst = {0, 365, 175, 175};
            SDL_RenderTexture(renderer, trackCoverArt, nullptr, &dst);
        }

        playbackControls->Render(renderer);

        SDL_RenderPresent(renderer);
    }

    delete topBar;
    delete songSelect;
    delete queueFrame;
    delete playbackControls;

    PlaybackQueue::Close();
    LibraryHandler::Close();

    if (trackCoverArt != nullptr) {
        SDL_DestroyTexture(trackCoverArt);
    }

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
        (*lib)->Insert(path);
        return;
    }

    if (info.type == SDL_PATHTYPE_DIRECTORY) {
        int count = 0;

        char **entries = SDL_GlobDirectory(path, nullptr, 0, &count);

        if (!entries) {
            printf("Error: %s\n", SDL_GetError());
            return;
        }

        for (int i = 0; i < count; i++) {
            if (std::string(entries[i]).find_last_of("/\\") == std::string::npos) {
                ScanLibrary(lib, std::string(path).append("/").append(entries[i]).c_str());
            }
        }

        SDL_free(entries);
    }
}

void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, const int index) {
    auto *b = frame->Add(0, static_cast<float>(50 * frame->NumChildren()), frame->GetRect().w, 50, 5, "Roboto.ttf", 50);
    b->SetText(renderer, trackName);
    b->AssignIndex(index);
    b->SetOnClickEvent(PlayInQueue, nullptr);  // TODO Determine if nullptr
    // TODO Probably introduce a method of parenting buttons to each other
    // TODO This is literally commented out because of how broken it is without parenting. It works, but not enough for me to even put in a dev build
    // auto *x = frame->Add(frame->GetRect().w - 50, b->GetFill().y, 50, 50, 5, "Roboto.ttf", 50);
    // x->SetText(renderer, "X");
    // x->AssignIndex(index);
    // x->SetOnClickEvent(RemoveFromQueue, frame);
}

void AddToQueue(void *info) {
    LibraryHandler::GetLibraryHandler()->QueueTrack(static_cast<TextButtonInfo *>(info)->textString.c_str());
    if (PlaybackQueue::GetPlaybackQueue()->GetQueueLength() == 1) {
        PlaybackQueue::GetPlaybackQueue()->Play(&playback, 0);
    }
}

void PlayInQueue(void *info) {
    PlaybackQueue::GetPlaybackQueue()->Play(&playback, static_cast<TextButtonInfo *>(info)->index);
}

// TODO This whole thing is extremely buggy and broken, mostly because of the lack of parenting
void RemoveFromQueue(void *info) {
    const int index = static_cast<TextButtonInfo *>(info)->index;
    auto *frame = static_cast<Frame *>(static_cast<TextButtonInfo *>(info)->userdata);
    PlaybackQueue::GetPlaybackQueue()->Remove(index);
    PlaybackQueue::GetPlaybackQueue()->Next(&playback);
    // TODO Need to remove same index twice since X is a child of frame rather than a child of the button being removed
    frame->Remove(index);
    frame->Remove(index);
}

void PlayPause(void *info) {
    if (playback == nullptr) {
        return;
    }

    if (playback->IsPaused()) {
        playback->Play();
    } else if (!playback->IsPaused()) {
        playback->Pause();
    }
}

void RR(void *info) {
    if (playback == nullptr) {
        return;
    }

    if (playback->GetRawPlaybackPosition() < 5) {
        PlaybackQueue::GetPlaybackQueue()->Play(&playback, PlaybackQueue::GetPlaybackQueue()->GetCurrentIndex() - 1);
    } else {
        playback->Restart();
    }
}

void FF(void *info) { // TODO This function should end the song if at the end of the queue
    skip = true;
}