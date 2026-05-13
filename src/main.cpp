#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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

    SDL_AudioSpec audio_spec;
    Uint8 *audio_buffer;
    Uint32 audio_buffer_length;

    SDL_AudioDeviceID dev = 0;
    SDL_AudioStream *stream = 0;

    if (!SDL_LoadWAV("./Heartbeat.wav", &audio_spec, &audio_buffer, &audio_buffer_length)) {
        std::cout << "Failed to load wav file: " << SDL_GetError() << std::endl;
        quit = true;
    } else {
        dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec);

        if (!dev) {
            std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
            SDL_free(audio_buffer);

            quit = true;
        } else {
            stream = SDL_CreateAudioStream(&audio_spec, &audio_spec);

            if (!stream) {
                std::cerr << "Failed to create audio stream: " << SDL_GetError() << std::endl;
                SDL_CloseAudioDevice(dev);
                SDL_free(audio_buffer);
                quit = true;
            } else {
                if (!SDL_BindAudioStream(dev, stream)) {
                    std::cerr << "Failed to bind audio stream: " << SDL_GetError() << std::endl;
                    SDL_DestroyAudioStream(stream);
                    SDL_CloseAudioDevice(dev);
                    SDL_free(audio_buffer);
                    quit = true;
                } else {
                    if (!SDL_PutAudioStreamData(stream, audio_buffer, audio_buffer_length)) {
                        SDL_DestroyAudioStream(stream);
                        SDL_CloseAudioDevice(dev);
                        SDL_free(audio_buffer);
                        quit = true;
                    }
                }
            }
        }
    }

    SDL_PauseAudioDevice(dev);

    bool paused = true;
    bool debounce = false;

    while (!quit) {
        if (SDL_GetKeyboardState(0)[SDL_SCANCODE_SPACE]) {
            if (!debounce) {
                debounce = true;
                if (paused) {
                    paused = false;
                    SDL_ResumeAudioDevice(dev);
                } else {
                    paused = true;
                    SDL_PauseAudioDevice(dev);
                }
            }
        } else {
            debounce = false;
        }

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;
                default: ;
            }
        }
    }

    if (stream)
        SDL_DestroyAudioStream(stream);
    if (dev)
        SDL_CloseAudioDevice(dev);
    SDL_free(audio_buffer);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
