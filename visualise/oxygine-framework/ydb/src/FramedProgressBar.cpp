#include "FramedProgressBar.h"
#include "ActorVisualise.h"
#include <iostream>


void FramedProgressBar::_init()
{
    _view->setPosition(_scene->getSize() / 2);

    frame_bg = new ColorRectSprite;
    frame_bg->setSize(944, 14);
    frame_bg->setColor(Color(255, 255, 255));
    frame_bg->attachTo(_view);
    frame_bg->setPosition(8, 640 - 24);

    bg = new ColorRectSprite;
    bg->setSize(940, 10);
    bg->setColor(Color(0, 0, 0));
    bg->attachTo(frame_bg);
    bg->setPosition(2, 2);

    progress_bar = new ProgressBar;
    progress_bar->setSize(940, 10);
    progress_bar->setPosition(0, 0);
    progress_bar->setProgress(0.f);
    progress_bar->attachTo(bg);
    progress_bar->setColor(Color(0, 255, 255));
}

void FramedProgressBar::setProgress(float val) {
    progress_bar->setProgress(val);
}

void FramedProgressBar::_update(const UpdateState& us)
{
}
