#include "TextButton.h"

#include <iostream>

#include "LibraryHandler.h"

TextButton::TextButton(const float x, const float y, const float w, const float h, const float thickness, const char *font, const float textSize)
    : Button{x, y, w, h, thickness} {
    SetFont(font, textSize);
}

TextButton::~TextButton() {
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void TextButton::SetTextColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
    textColor.r = r;
    textColor.g = g;
    textColor.b = b;
    textColor.a = a;
}

void TextButton::SetFont(const char *fontPath, const float size) {
    font = TTF_OpenFont(fontPath, size);
}

void TextButton::SetText(SDL_Renderer *renderer, const char *text) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, textColor);
    if (!surface) {
        std::cerr << "Error creating surface: " << SDL_GetError() << "\n";
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << "\n";
    }

    SDL_DestroySurface(surface);

    textString = text;
}

void TextButton::Render(SDL_Renderer *renderer) {
    Button::Render(renderer);

    SDL_RenderTexture(renderer, texture, nullptr, &innerRect);
}

void TextButton::OnClick(const float x, const float y) {
    if (IsInBounds(x, y)) {
        LibraryHandler::GetLibraryHandler()->QueueTrack(textString.c_str());
    }
}