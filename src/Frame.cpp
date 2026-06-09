#include "Frame.h"

Frame::Frame(const float x, const float y, const float w, const float h, const int frameType)
{
    Move(x, y);
    Resize(w, h);

    type = frameType;
}

Frame::~Frame() {
    for (auto &button : buttons) {
        delete button;
        button = nullptr;
    }

    buttons.clear();
    buttons.shrink_to_fit();
}

void Frame::Move(const float x, const float y) {
    rect.x = x;
    rect.y = y;

    for (const auto &button : buttons) {
        button->SetPos(x + button->GetBorder().x, y + button->GetBorder().y);
    }
}

void Frame::Resize(const float w, const float h) {
    rect.w = w;
    rect.h = h;
}

void Frame::SetColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
}

Button *Frame::Add(const float x, const float y, const float w, const float h, const float thickness) {
    auto *b = new Button(x + rect.x, y + rect.y, w, h, thickness);
    buttons.push_back(b);
    return b;
}

TextButton *Frame::Add(const float x, const float y, const float w, const float h, const float thickness, const char *font, const float textSize) {
    auto *b = new TextButton(x + rect.x, y + rect.y, w, h, thickness, font, textSize);
    buttons.push_back(b);
    return b;
}

void Frame::Render(SDL_Renderer *renderer) const {
    SDL_Color restore;
    SDL_GetRenderDrawColor(renderer, &restore.r, &restore.g, &restore.b, &restore.a);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, restore.r, restore.g, restore.b, restore.a);
    for (const auto &button : buttons) {
        button->Render(renderer);
    }
}

void Frame::HandleEvent(const SDL_Event &e) const {
    switch (e.type)
    {
        case SDL_EVENT_MOUSE_MOTION:
            for (const auto & button : buttons) {
                button->Hover(e.motion.x, e.motion.y);
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                for (const auto & button : buttons) {
                    button->OnClick(e.motion.x, e.motion.y, type);
                }
            }
            break;
        default: ;
    }
}

unsigned int Frame::NumChildren() const {
    return static_cast<unsigned int>(buttons.size());
}

SDL_FRect Frame::GetRect() const {
    return rect;
}

bool Frame::IsInBounds(const float x, const float y) const {
    return (x > rect.x && x < rect.x + rect.w) && (y > rect.y && y < rect.y + rect.h);
}