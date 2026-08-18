#include "Button.h"

Button::Button(SDL_Window *window, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness)
    : Frame(window, x, y, w, h)
{
    SetBorderThickness(thickness);

    activeFill = &buttonColor;
    activeBorder = &borderColor;
}

Button::Button(Frame *parent, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness)
    : Frame(parent, x, y, w, h)
{
    SetBorderThickness(thickness);

    activeFill = &buttonColor;
    activeBorder = &borderColor;
}

void Button::Render(SDL_Renderer *renderer)
{
    SDL_Color restore;
    SDL_GetRenderDrawColor(renderer, &restore.r, &restore.g, &restore.b, &restore.a);
    SDL_SetRenderDrawColor(renderer, activeBorder->r, activeBorder->g, activeBorder->b, activeBorder->a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, activeFill->r, activeFill->g, activeFill->b, activeFill->a);
    SDL_RenderFillRect(renderer, &innerRect);
    SDL_SetRenderDrawColor(renderer, restore.r, restore.g, restore.b, restore.a);
    for (const auto &child : children)
    {
        child->Render(renderer);
    }
}

void Button::Move(const ScaleOffset x, const ScaleOffset y)
{
    const float deltaX = innerRect.x - rect.x;
    const float deltaY = innerRect.y - rect.y;

    Frame::Move(x, y);

    innerRect.x = rect.x + deltaX;
    innerRect.y = rect.y + deltaY;
}

void Button::Resize(const ScaleOffset w, const ScaleOffset h)
{
    const float thick = (rect.w - innerRect.w) / 2;

    Frame::Resize(w, h);

    SetBorderThickness(thick);
}

void Button::SetButtonColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    buttonColor.r = r;
    buttonColor.g = g;
    buttonColor.b = b;
    buttonColor.a = a;
}

void Button::SetBorderColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    borderColor.r = r;
    borderColor.g = g;
    borderColor.b = b;
    borderColor.a = a;
}

void Button::SetButtonHoverColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    buttonHoverColor.r = r;
    buttonHoverColor.g = g;
    buttonHoverColor.b = b;
    buttonHoverColor.a = a;
}

void Button::SetBorderHoverColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    borderHoverColor.r = r;
    borderHoverColor.g = g;
    borderHoverColor.b = b;
    borderHoverColor.a = a;
}

void Button::SetBorderThickness(const float newThick)
{
    if (newThick < 0)
    {
        innerRect.x = rect.x;
        innerRect.y = rect.y;
        innerRect.w = rect.w;
        innerRect.h = rect.h;
    }
    else
    {
        innerRect.x = rect.x + newThick;
        innerRect.y = rect.y + newThick;
        innerRect.w = rect.w - (newThick * 2);
        innerRect.h = rect.h - (newThick * 2);
    }
}

void Button::Hover(const float x, const float y)
{
    if (IsInBounds(x, y))
    {
        activeFill = &buttonHoverColor;
        activeBorder = &borderHoverColor;
    }
    else
    {
        activeFill = &buttonColor;
        activeBorder = &borderColor;
    }
}

SDL_FRect Button::GetBorder() const
{
    return GetRect();
}

SDL_FRect Button::GetFill() const
{
    return innerRect;
}

void Button::HandleEvent(const SDL_Event &e)
{
    if (e.type == SDL_EVENT_MOUSE_MOTION)
    {
        Hover(e.motion.x, e.motion.y);
    }
    else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (e.button.button == SDL_BUTTON_LEFT)
        {
            OnClick(e.motion.x, e.motion.y);
        }
    }

    for (const auto &child : children)
    {
        child->HandleEvent(e);
    }
}

void Button::SetOnClickEvent(void (*event)(void *), void *userdata)
{
    clickEvent = event;
    data = userdata;
}