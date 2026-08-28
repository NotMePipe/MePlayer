#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "LibraryHandler.h"
#include "PlaybackQueue.h"
#include "ScrollingFrame.h"
#include "TextButton.h"
#include "Utils.h"

void ScanLibrary(LibraryHandler **lib, const char *path);
void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, int index);
void AddToQueue(void *info);
void PlayInQueue(void *info);
void RemoveFromQueue(void *info);
void Seek(void *info);
// TODO These need overhauled
void PlayPause(void *info);
void RR(void *info);
void FF(void *info);

Track *playback;
// TODO Temporary playback handlers
bool skip = false;

int main(int argc, char* argv[])
{
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

    auto *mainScreen = new Frame(window, {0, 0}, {0, 0}, {1, 0}, {1, 0});

    auto *topBar = new Button(mainScreen, {0, 0}, {0, 0}, {1, 0}, {0.056, 0}, -1);
    topBar->SetButtonColor(255, 255, 255, 255);

    auto *songSelect = new ScrollingFrame(mainScreen, {0, 0}, {0.056, 0}, {0.781, 0}, {0.807, 0});
    {
        int count = 0;
        for (const auto &[path, name] : library_handler->GetAllInfo())
        {
            // TODO Make the linter not as angry at me, but this is currently mostly testing code
            float padding = 0.01;
            float height = 0.1;
            int buttonsPerRow = 3;

            int col = count % buttonsPerRow;
            int row = floor((count++) / buttonsPerRow);

            auto *b = new TextButton(songSelect, {col * (1.0f / buttonsPerRow) + (padding / 2), 0}, {row * (height + padding), 0}, {(1.0f / buttonsPerRow) - padding, 0}, {height, 0}, 5.0f, "Roboto.ttf", 50.0f);

            b->SetText(renderer, name.c_str());
            b->SetOnClickEvent(AddToQueue, nullptr); // TODO Determine if nullptr
        }
    }

    auto *queueFrame = new ScrollingFrame(mainScreen, {0.781, 0}, {0.056, 0}, {0.219, 0}, {0.807, 0});
    playback_queue->SetQueueAddCallback(renderer, queueFrame, AddQueueButton);

    auto *playbackControls = new Frame(mainScreen, {0.182, 0}, {0.861, 0}, {0.818, 0}, {0.139, 0});
    playbackControls->SetColor(255, 0, 0, 255);
    // TODO These should all become image buttons
    {
        auto *playPause = new TextButton(playbackControls, {0.289, 0}, {0.53, 0}, {0.2, 0}, {0.3, 0}, 5, "Roboto.ttf", 50);
        playPause->SetText(renderer, "Play/Pause");
        playPause->SetOnClickEvent(PlayPause, nullptr); // TODO Determine if nullptr

        auto *rr = new TextButton(playbackControls, {0.064, 0}, {0.53, 0}, {0.2, 0}, {0.3, 0}, 5, "Roboto.ttf", 50);
        rr->SetText(renderer, "RR");
        rr->SetOnClickEvent(RR, nullptr); // TODO Determine if nullptr

        auto *ff = new TextButton(playbackControls, {0.515, 0}, {0.53, 0}, {0.2, 0}, {0.3, 0}, 5, "Roboto.ttf", 50);
        ff->SetText(renderer, "FF");
        ff->SetOnClickEvent(FF, nullptr);  // TODO Determine if nullptr
    }

    auto *timeBack = new Button(playbackControls, {0.063, 0}, {0.1, 0}, {0.652, 0}, {0.3, 0}, 0);
    timeBack->SetBorderColor(0, 0, 0, 0);
    timeBack->SetBorderHoverColor(0, 0, 0, 0);
    timeBack->SetButtonColor(0, 0, 0, 255);
    timeBack->SetButtonHoverColor(0, 0, 0, 255);

    auto *timeProg = new Button(timeBack, {0, 0}, {0, 0}, {0, 0}, {1, 0}, 0);
    timeProg->SetBorderColor(0, 0, 0, 0);
    timeProg->SetBorderHoverColor(0, 0, 0, 0);
    timeProg->SetButtonColor(255, 255, 255, 255);
    timeProg->SetButtonHoverColor(255, 255, 255, 255);

    float mousePercent = 0;
    timeBack->SetOnClickEvent(Seek, &mousePercent);

    playback = nullptr;
    bool quit = false;
    SDL_Event event;
    SDL_Texture *trackCoverArt = nullptr;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:
                    SDL_GetWindowSizeInPixels(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

                    mainScreen->Resize({1, 0}, {1, 0});
                    mainScreen->Move({0, 0}, {0, 0});
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    songSelect->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    queueFrame->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    mousePercent = (event.motion.x - timeBack->GetBorder().x) / ((timeBack->GetBorder().x + timeBack->GetBorder().w) - timeBack->GetBorder().x);
                    break;
                default:
                    if (event.type == TRACK_PLAY_EVENT)
                    {
                        trackCoverArt = SDL_CreateTextureFromSurface(renderer, playback->CoverArt());
                    }
                    else if (event.type == TRACK_DESTROY_EVENT)
                    {
                        SDL_DestroyTexture(trackCoverArt);
                        trackCoverArt = nullptr;
                    }
                    else
                    {
                        mainScreen->HandleEvent(event);
                    }
            }
        }

        if (playback != nullptr)
        {
            if (playback->TrackEnded() || skip)
            {
                skip = false;
                if (playback_queue->Next(&playback) < 0)
                {
                    playback->Pause();
                }
            }
            timeProg->Resize({static_cast<float>(playback->GetRawPlaybackPosition()) / static_cast<float>(playback->GetRawTrackLength())}, {1, 0});
        }
        else
        {
            if (playback_queue->Next(&playback) == 0)
            {
                playback->Play();
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        mainScreen->Render(renderer);

        // TODO This should be handled with image button
        if (trackCoverArt != nullptr)
        {
            SDL_FRect dst = {0, 365, 175, 175};
            SDL_RenderTexture(renderer, trackCoverArt, nullptr, &dst);
        }

        SDL_RenderPresent(renderer);
    }

    delete mainScreen;

    PlaybackQueue::Close();
    LibraryHandler::Close();

    if (trackCoverArt != nullptr)
    {
        SDL_DestroyTexture(trackCoverArt);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

void ScanLibrary(LibraryHandler **lib, const char *path)
{
    SDL_PathInfo info;

    if (!SDL_GetPathInfo(path, &info))
    {
        std::cerr << "Error in GetPathInfo: " << SDL_GetError() << " : " << path << "\n";
        return;
    }

    if (info.type == SDL_PATHTYPE_FILE)
    {
        (*lib)->Insert(path);
        return;
    }

    if (info.type == SDL_PATHTYPE_DIRECTORY)
    {
        int count = 0;

        char **entries = SDL_GlobDirectory(path, nullptr, 0, &count);

        if (!entries)
        {
            printf("Error: %s\n", SDL_GetError());
            return;
        }

        for (int i = 0; i < count; i++)
        {
            if (std::string(entries[i]).find_last_of("/\\") == std::string::npos)
            {
                ScanLibrary(lib, std::string(path).append("/").append(entries[i]).c_str());
            }
        }

        SDL_free(entries);
    }
}

void AddQueueButton(SDL_Renderer *renderer, Frame *frame, const char *trackName, const int index)
{
    auto *b = new TextButton(frame, {0, 0}, {static_cast<float>(frame->NumChildren()) * 0.11f}, {1, 0}, {0.1f, 0}, 5, "Roboto.ttf", 50);
    b->SetText(renderer, trackName);
    b->AssignIndex(index);
    b->SetOnClickEvent(PlayInQueue, nullptr);  // TODO Determine if nullptr

    // TODO I would like to find a better solution for this in all areas
    auto *x = new TextButton(b, {1, -50}, {0, 0}, {0, 50}, {0, 50}, 5, "Roboto.ttf", 50);
    // TODO Figure out render order when text is involved
    x->SetText(renderer, "X");
    x->AssignIndex(index);
    x->SetOnClickEvent(RemoveFromQueue, frame);
}

void AddToQueue(void *info)
{
    LibraryHandler::GetLibraryHandler()->QueueTrack(static_cast<TextButtonInfo *>(info)->textString.c_str());
    if (PlaybackQueue::GetPlaybackQueue()->GetQueueLength() == 1)
    {
        PlaybackQueue::GetPlaybackQueue()->Play(&playback, 0);
    }
}

void PlayInQueue(void *info)
{
    PlaybackQueue::GetPlaybackQueue()->Play(&playback, static_cast<TextButtonInfo *>(info)->index);
}

void Seek(void *info)
{
    if (playback != nullptr)
    {
        // TODO My linter is upset about this, but I am tempted to go through and remove most of the static_casts and make it suck it up
        playback->Seek(playback->GetRawTrackLength() * *static_cast<float *>(info));
    }
}

// TODO I need to implement a way to intelligently move the rest of the queue to the top when an item is removed
void RemoveFromQueue(void *info) // TODO This randomly started crashing again, but this whole system needs fixed
{
    const int index = static_cast<TextButtonInfo *>(info)->index;
    auto *frame = static_cast<Frame *>(static_cast<TextButtonInfo *>(info)->userdata);
    PlaybackQueue::GetPlaybackQueue()->Remove(index);
    PlaybackQueue::GetPlaybackQueue()->Next(&playback);
    frame->RemoveChild(index);
}

void PlayPause(void *info)
{
    if (playback == nullptr)
    {
        return;
    }

    if (playback->IsPaused())
    {
        playback->Play();
    }
    else if (!playback->IsPaused())
    {
        playback->Pause();
    }
}

void RR(void *info)
{
    if (playback == nullptr)
    {
        return;
    }

    if (playback->GetRawPlaybackPosition() < 5)
    {
        PlaybackQueue::GetPlaybackQueue()->Play(&playback, PlaybackQueue::GetPlaybackQueue()->GetCurrentIndex() - 1);
    }
    else
    {
        playback->Restart();
    }
}

void FF(void *info) // TODO This function should end the song if at the end of the queue
{
    skip = true;
}