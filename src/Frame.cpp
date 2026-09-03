#include "Frame.h"

#include "Utils.h"

Frame::Frame(SDL_Window *window, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
{
    this->window = window;
    parent = nullptr;
    Frame::Move(x, y);
    Frame::Resize(w, h);
}

Frame::Frame(Frame *parent, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
{
    window = nullptr;
    this->parent = parent;
    parent->AddChild(this);
    Frame::Move(x, y);
    Frame::Resize(w, h);
}

Frame::~Frame()
{
    for (const auto &child : children)
    {
        delete child;
    }

    children.clear();
}

void Frame::Move(const ScaleOffset x, const ScaleOffset y)
{
    float targetX, targetY;
    if (parent == nullptr)
    {
        targetX = (static_cast<float>(SCREEN_WIDTH) * x.scale) + x.offset;
        targetY = (static_cast<float>(SCREEN_HEIGHT) * y.scale) + y.offset;
    }
    else
    {
        targetX = parent->rect.x + (parent->rect.w * x.scale) + x.offset;
        targetY = parent->rect.y + (parent->rect.h * y.scale) + y.offset;
    }

    rect.x = targetX;
    rect.y = targetY;

    position[0] = x;
    position[1] = y;

    for (const auto &child : children)
    {
        child->Move(child->position[0], child->position[1]);
    }
}

void Frame::Resize(const ScaleOffset w, const ScaleOffset h)
{
    float targetW, targetH;
    if (parent == nullptr)
    {
        targetW = (static_cast<float>(SCREEN_WIDTH) * w.scale) + w.offset;
        targetH = (static_cast<float>(SCREEN_HEIGHT) * h.scale) + h.offset;
    }
    else
    {
        targetW = (parent->rect.w * w.scale) + w.offset;
        targetH = (parent->rect.h * h.scale) + h.offset;
    }

    rect.w = targetW;
    rect.h = targetH;

    size[0] = w;
    size[1] = h;

    for (const auto &child : children)
    {
        child->Resize(child->size[0], child->size[1]);
        child->Move(child->position[0], child->position[1]);
    }
}

void Frame::SetColor(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
}

void Frame::AddChild(Frame *child)
{
    children.push_back(child);
}

void Frame::RemoveChild(const int index)
{
    if (index < children.size())
    {
        delete children.at(index);
        children.erase(children.begin() + index);
    }
}

void Frame::Render(SDL_Renderer *renderer)
{
    SDL_Color restore;
    SDL_GetRenderDrawColor(renderer, &restore.r, &restore.g, &restore.b, &restore.a);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, restore.r, restore.g, restore.b, restore.a);
    for (const auto &child : children)
    {
        child->Render(renderer);
    }
}

void Frame::HandleEvent(const SDL_Event &e)
{
    for (const auto &child : children)
    {
        child->HandleEvent(e);
    }
}

unsigned int Frame::NumChildren() const
{
    return static_cast<unsigned int>(children.size());
}

SDL_FRect Frame::GetRect() const
{
    return rect;
}

ScaleOffset *Frame::GetPosition() {
    return position;
}

ScaleOffset *Frame::GetSize() {
    return size;
}

bool Frame::IsInBounds(const float x, const float y) const // TODO Layer checking is NEEDED
{
    return (x > rect.x && x < rect.x + rect.w) && (y > rect.y && y < rect.y + rect.h);
}