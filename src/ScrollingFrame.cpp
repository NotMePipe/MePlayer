#include "ScrollingFrame.h"

ScrollingFrame::ScrollingFrame(SDL_Window *window, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
    : Frame{window, x, y, w, h} {
}

ScrollingFrame::ScrollingFrame(Frame *parent, const ScaleOffset x, const ScaleOffset y, const ScaleOffset w, const ScaleOffset h)
    : Frame{parent, x, y, w, h} {
}

void ScrollingFrame::Scroll(float x, float y, float amount) { // TODO Add bounds
    if (IsInBounds(x, y)) {
        for (const auto &child : children) {
            // This will not work because it doesn't update children positions
            // Current theory is to store the ScaleOffset data and directly modify that
            // I stopped early because that would encourage redoing the Move and Resize logic (again) and I wanted to commit first
            //child->SetPos(child->GetRect().x, child->GetRect().y + amount);
            auto [scale, offset] = child->GetPosition()[1];
            child->Move(child->GetPosition()[0], {scale, offset + amount});
        }
    }
}