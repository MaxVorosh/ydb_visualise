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
    void set_color(Color c);

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
    virtual void onClick(Event* ev);
    void set_type(std::string);
    void set_style(TextStyle style);
    void set_scale(float scale);
    void set_color(Color c);
protected:
    spArrowPart _body;
    spArrowPart _left_part;
    spArrowPart _right_part;
    spTextField _label;
    bool visible_text = true;
    float scale = 1.0;
    float label_x;
    float label_y;
};

DECLARE_SMART(MainArrow, spMainArrow);
class MainArrow: public Arrow {
public:
    void onClick(Event* ev) override;
    bool is_locked();
protected:
    bool locked = false;
};