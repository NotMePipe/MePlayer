#ifndef FRAME_H
#define FRAME_H

#include <vector>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_events.h>

typedef struct ScaleOffset {
    float scale;
    float offset;
} ScaleOffset;

class Frame {
public:
    Frame(SDL_Window *window, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h);
    Frame(Frame *parent, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h);
    virtual ~Frame();

    virtual void Move(ScaleOffset x, ScaleOffset y);
    virtual void Resize(ScaleOffset w, ScaleOffset h);

    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void AddChild(Frame *child);
    void RemoveChild(int index);

    virtual void Render(SDL_Renderer *renderer);

    virtual void HandleEvent(const SDL_Event &e);

    [[nodiscard]] unsigned int NumChildren() const;

    [[nodiscard]] SDL_FRect GetRect() const;

private:
    Frame *parent;
    SDL_Window *window;

    void SetPos(float x, float y);
    void SetSize(float w, float h);

protected:
    SDL_FRect rect{};
    SDL_Color color{0, 0, 0, 255};

    std::vector<Frame *> children;

    [[nodiscard]] bool IsInBounds(float x, float y) const;
};

#endif //FRAME_H
