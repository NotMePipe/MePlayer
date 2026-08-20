#include "ScrollingFrame.h"

ScrollingFrame::ScrollingFrame(SDL_Window *window, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
    : Frame{window, x, y, w, h}
{}

ScrollingFrame::ScrollingFrame(Frame *parent, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
    : Frame{parent, x, y, w, h}
{}

// TODO Add bounds
void ScrollingFrame::Scroll(float x, float y, float amount)
{
    if (IsInBounds(x, y))
    {
        for (const auto &child : children)
        {
            auto [scale, offset] = child->GetPosition()[1];
            child->Move(child->GetPosition()[0], {scale, offset + amount});
        }
    }
}