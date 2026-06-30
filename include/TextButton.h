#ifndef TEXT_BUTTON_H
#define TEXT_BUTTON_H

#include <string>

#include <SDL3_ttf/SDL_ttf.h>

#include "Button.h"

typedef struct ButtonInfo {
    std::string textString;
    int index;
    void *userdata; // TODO This is here just to keep things working until parents are fully supported
} TextButtonInfo;

class TextButton : public Button {
public:
    TextButton(float x, float y, float w, float h, float thickness, const char *font, float textSize);
    ~TextButton() override;

    void SetTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void SetFont(const char *fontPath, float size);

    void SetText(SDL_Renderer *renderer, const char *text);

    void Render(SDL_Renderer *renderer) override;

    void OnClick(float x, float y) override;

    void AssignIndex(int i);

private:
    TTF_Font *font = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Color textColor{255, 255, 255, 255};

    TextButtonInfo info{};
};

#endif //TEXT_BUTTON_H
