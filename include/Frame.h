#ifndef FRAME_H
#define FRAME_H

#include <vector>

#include <SDL3/SDL_rect.h>

#include "TextButton.h"

class Frame {
public:
    Frame(float x, float y, float w, float h);
    ~Frame();

    void Move(float x, float y);
    void Resize(float w, float h);
    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    Button *Add(float x, float y, float w, float h, float thickness);
    TextButton *Add(float x, float y, float w, float h, float thickness, const char *font, float textSize);

    void Render(SDL_Renderer *renderer) const;

    void HandleEvent(const SDL_Event &e) const;

    [[nodiscard]] unsigned int NumChildren() const;

private:
    SDL_FRect rect{};
    SDL_Color color{0, 0, 0, 255};

    std::vector<Button *> buttons;
};

#endif //FRAME_H
