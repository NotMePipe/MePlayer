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

void Frame::SetPos(const float x, const float y)
{
    rect.x = x;
    rect.y = y;
}

void Frame::SetSize(const float w, const float h)
{
    rect.w = w;
    rect.h = h;
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

    for (const auto &child : children)
    {
        const float deltaX = child->rect.x - rect.x;
        const float deltaY = child->rect.y - rect.y;

        child->SetPos(targetX + deltaX, targetY + deltaY);
    }

    rect.x = targetX;
    rect.y = targetY;
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

    for (const auto &child : children)
    {
        const float percentW = child->rect.w / rect.w;
        const float percentH = child->rect.h / rect.h;

        const float relX = (child->rect.x - rect.x) / rect.w;
        const float relY = (child->rect.y - rect.y) / rect.h;

        child->SetSize(targetW * percentW, targetH * percentH);
        child->SetPos(rect.x + relX * targetW, rect.y + relY * targetH);
    }

    rect.w = targetW;
    rect.h = targetH;
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

bool Frame::IsInBounds(const float x, const float y) const
{
    return (x > rect.x && x < rect.x + rect.w) && (y > rect.y && y < rect.y + rect.h);
}