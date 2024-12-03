#pragma once
#include "Unit.h"
#include "ActorVisualise.h"
#include "oxygine-framework.h"
#include <string>

DECLARE_SMART(ArrowPart, spArrowPart);
class ArrowPart: public Unit
{
public:
    void rotate(float angle);
    void scaleY(float new_x);
    void move(float x, float y);
    void enable();
    void disable();
    void set_scale(float scale);

    spSprite _click_box;

protected:
    void _init();
    void _update(const UpdateState& us);

    spSprite _body;
    float scale = 1.0;
    float size_x;
    float x;
    float y;
};

DECLARE_SMART(Arrow, spArrow);
class Arrow: public Object {
public:
    void init(ActorVisualise* scene);
    void point(int x_from, int y_from, int x_to, int y_to);
    void enable();
    void disable();
    void onClick(Event* ev);
    void set_type(std::string);
    void set_style(TextStyle style);
    void set_scale(float scale);
private:
    spArrowPart _body;
    spArrowPart _left_part;
    spArrowPart _right_part;
    spTextField _label;
    bool visible_text = false;
    float scale = 1.0;
};

