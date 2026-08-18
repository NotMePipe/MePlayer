#ifndef SCROLLING_FRAME_H
#define SCROLLING_FRAME_H

#include "Frame.h"

class ScrollingFrame : public Frame {
public:
    ScrollingFrame(SDL_Window *window, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h);
    ScrollingFrame(Frame *parent, ScaleOffset x, ScaleOffset y, ScaleOffset w, ScaleOffset h);

    void Scroll(float x, float y, float amount);
};

#endif //SCROLLING_FRAME_H
