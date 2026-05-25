#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL_render.h>

class Button {
public:
    Button(float x, float y, float w, float h, float thickness);
    virtual ~Button() = default;

    virtual void Render(SDL_Renderer *renderer);

    void SetPos(float newX, float newY);
    void SetSize(float newW, float newH);

    void SetButtonColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void SetBorderColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void SetButtonHoverColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void SetBorderHoverColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void SetBorderThickness(float newThick);

    void Hover(float x, float y);

    virtual void OnClick(float x, float y) {}

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

    [[nodiscard]] bool IsInBounds(float x, float y) const;
};


#endif //BUTTON_H
