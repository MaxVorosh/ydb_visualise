#pragma once
#include "Unit.h"

DECLARE_SMART(FramedProgressBar, spFramedProgressBar);
class FramedProgressBar: public Unit
{
public:
    void setProgress(float val);
protected:
    void _init();
    void _update(const UpdateState& us);

    spProgressBar progress_bar;
    spColorRectSprite frame_bg;
    spColorRectSprite bg;
};

