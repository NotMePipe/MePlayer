#include "ScrollingFrame.h"

ScrollingFrame::ScrollingFrame(const float x, const float y, const float w, const float h, const int frameType)
    : Frame{x, y, w, h, frameType} {
}

void ScrollingFrame::Scroll(float x, float y, float amount) { // TODO Add bounds
    if (IsInBounds(x, y)) {
        for (const auto &button : buttons) {
            button->SetPos(button->GetBorder().x, button->GetBorder().y - amount);
        }
    }
}