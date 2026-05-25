#include "Button.h"

Button::Button(const float x, const float y, const float w, const float h, const float thickness) {
    SetPos(x, y);
    SetSize(w, h);
    SetBorderThickness(thickness);
    
    activeFill = &buttonColor;
    activeBorder = &borderColor;
}

void Button::Render(SDL_Renderer *renderer) {
    SDL_Color restore;
    SDL_GetRenderDrawColor(renderer, &restore.r, &restore.g, &restore.b, &restore.a);
    SDL_SetRenderDrawColor(renderer, activeBorder->r, activeBorder->g, activeBorder->b, activeBorder->a);
    SDL_RenderFillRect(renderer, &borderRect);
    SDL_SetRenderDrawColor(renderer, activeFill->r, activeFill->g, activeFill->b, activeFill->a);
    SDL_RenderFillRect(renderer, &innerRect);
    SDL_SetRenderDrawColor(renderer, restore.r, restore.g, restore.b, restore.a);
}

void Button::SetPos(const float newX, const float newY) {
    borderRect.x = newX;
    borderRect.y = newY;
}

void Button::SetSize(const float newW, const float newH) {
    borderRect.w = newW;
    borderRect.h = newH;
}

void Button::SetButtonColor(const Uint8 r,const Uint8 g,const Uint8 b,const Uint8 a) {
    buttonColor.r = r;
    buttonColor.g = g;
    buttonColor.b = b;
    buttonColor.a = a;
}

void Button::SetBorderColor(const Uint8 r,const Uint8 g,const Uint8 b,const Uint8 a) {
    borderColor.r = r;
    borderColor.g = g;
    borderColor.b = b;
    borderColor.a = a;
}

void Button::SetButtonHoverColor(const Uint8 r,const Uint8 g,const Uint8 b,const Uint8 a) {
    buttonHoverColor.r = r;
    buttonHoverColor.g = g;
    buttonHoverColor.b = b;
    buttonHoverColor.a = a;
}

void Button::SetBorderHoverColor(const Uint8 r,const Uint8 g,const Uint8 b,const Uint8 a) {
    borderHoverColor.r = r;
    borderHoverColor.g = g;
    borderHoverColor.b = b;
    borderHoverColor.a = a;
}

void Button::SetBorderThickness(const float newThick) {
    innerRect.x = borderRect.x + newThick;
    innerRect.y = borderRect.y + newThick;
    innerRect.w = borderRect.w - (newThick * 2);
    innerRect.h = borderRect.h - (newThick * 2);
}

bool Button::IsInBounds(const float x, const float y) const {
     return (x > borderRect.x && x < borderRect.x + borderRect.w) && (y > borderRect.y && y < borderRect.y + borderRect.h);
}

void Button::Hover(const float x, const float y) {
    if (IsInBounds(x, y)) {
        activeFill = &buttonHoverColor;
        activeBorder = &borderHoverColor;
    } else {
        activeFill = &buttonColor;
        activeBorder = &borderColor;
    }
}