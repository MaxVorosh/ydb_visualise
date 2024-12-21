#pragma once
#include "Unit.h"
#include <string>

DECLARE_SMART(NavigationButton, spNavigationButton);
class NavigationButton: public Unit
{
public:
    void move(int x, int y);
    void bind_pictures(ResAnim* main_anim, ResAnim* other_anim);
    bool get_state();
    void reset();
    void switch_picture();

protected:
    void _init();
    void _update(const UpdateState& us);
    void onClick(Event* ev);

    spSprite _body;
    bool state = false;
    ResAnim* main_anim;
    ResAnim* other_anim;
};

