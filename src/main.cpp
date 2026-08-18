#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "ScrollingFrame.h"
#include "TextButton.h"
#include "Utils.h"

// TODO Replace (ALL) constant sizing and positioning with dynamic scaling
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

    bool quit = false;
    SDL_Event event;

    auto *sf = new ScrollingFrame(window, {.5, 0}, {.5, 0}, {.5, 0}, {.5, 0});
    sf->SetColor(255, 255, 0, 255);

    auto *f = new Frame(sf, {.5, 0}, {.5, 0}, {.5, 0}, {.5, 0});
    f->SetColor(0, 255, 0, 255);

    auto *b = new Button(f, {.5, 0}, {.5, 0}, {.5, 0}, {.5, 0}, 10);
    b->SetBorderColor(0, 0, 255, 255);
    b->SetButtonColor(255, 0, 0, 255);
    b->SetBorderHoverColor(0, 0, 255, 255);
    b->SetButtonHoverColor(255, 0, 0, 255);

    auto *tb = new TextButton(b, {.5, 0}, {.5, 0}, {.5, 0}, {.5, 0}, 10, "Roboto.ttf", 50);
    tb->SetBorderColor(0, 255, 0, 255);
    tb->SetButtonColor(255, 0, 255, 255);
    tb->SetText(renderer, "test");

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
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    sf->Scroll(event.wheel.mouse_x, event.wheel.mouse_y, event.wheel.y * SCROLL_SCALE);
                default:
                    sf->HandleEvent(event);
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        sf->Render(renderer);

        SDL_RenderPresent(renderer);
    }

    delete sf;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}