#include "TextButton.h"

#include <iostream>

#include "LibraryHandler.h"
#include "PlaybackQueue.h"

TextButton::TextButton(SDL_Window *window, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness, const char *font, float textSize)
    : Button{window, x, y, w, h, thickness}
{
    SetFont(font, textSize);
    info.index = -1;
}

TextButton::TextButton(Frame *parent, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness, const char *font, float textSize)
    : Button{parent, x, y, w, h, thickness}
{
    SetFont(font, textSize);
    info.index = -1;
}

TextButton::~TextButton() {
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void TextButton::SetTextColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    textColor.r = r;
    textColor.g = g;
    textColor.b = b;
    textColor.a = a;
}

void TextButton::SetFont(const char *fontPath, const float size)
{
    font = TTF_OpenFont(fontPath, size);
}

void TextButton::SetText(SDL_Renderer *renderer, const char *text)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, textColor);
    if (!surface)
    {
        std::cerr << "Error creating surface: " << SDL_GetError() << "\n";
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        std::cerr << "Error creating texture: " << SDL_GetError() << "\n";
    }

    SDL_DestroySurface(surface);

    info.textString = text;
}

void TextButton::Render(SDL_Renderer *renderer)
{
    Button::Render(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, &innerRect);
}

void TextButton::OnClick(const float x, const float y)
{
    if (IsInBounds(x, y))
    {
        if (clickEvent != nullptr)
        {
            info.userdata = data;
            clickEvent(&info);
        }
    }
}

void TextButton::AssignIndex(const int i)
{
    info.index = i;
}