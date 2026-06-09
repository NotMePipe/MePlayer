#ifndef SCROLLING_FRAME_H
#define SCROLLING_FRAME_H

#include "Frame.h"

class ScrollingFrame : public Frame {
public:
    ScrollingFrame(float x, float y, float w, float h, int frameType = 0);

    void Scroll(float x, float y, float amount);
};

#endif //SCROLLING_FRAME_H
