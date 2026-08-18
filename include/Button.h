#ifndef BUTTON_H
#define BUTTON_H

#include "Frame.h"

class Button : public Frame {
public:
    Button(SDL_Window *window, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness);
    Button(Frame *parent, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h, float thickness);
    ~Button() override = default;

    void Render(SDL_Renderer *renderer) override;

    void Move(ScaleOffset x, ScaleOffset y) override;
    void Resize(ScaleOffset w, ScaleOffset h) override;

    void SetButtonColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void SetBorderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void SetButtonHoverColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void SetBorderHoverColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void SetBorderThickness(float newThick);

    void Hover(float x, float y);

    virtual void OnClick(float x, float y) {}

    [[nodiscard]] SDL_FRect GetBorder() const;
    [[nodiscard]] SDL_FRect GetFill() const;

    void HandleEvent(const SDL_Event &e) override;

    void SetOnClickEvent(void (*event)(void *), void *userdata);

private:
    SDL_Color buttonColor{0, 0, 0, 0};
    SDL_Color borderColor{255, 255, 255, 255};

    SDL_Color buttonHoverColor{0, 0, 0, 0};
    SDL_Color borderHoverColor{255, 255, 255, 255};

    SDL_Color *activeFill;
    SDL_Color *activeBorder;

protected:
    SDL_FRect borderRect{};
    SDL_FRect innerRect{};

    void (*clickEvent)(void *) = nullptr;
    void *data = nullptr;
};


#endif //BUTTON_H
