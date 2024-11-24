#pragma once
#include "Unit.h"
#include <string>

DECLARE_SMART(ActorModel, spActorModel);
class ActorModel: public Unit
{
public:
    void move(int x, int y);
    void resize(int x, int y);
    void activate();
    void deactivate();
    void disable();
    void enable();
    void rename(std::string new_name);
    void set_activity_type(std::string new_activity_type);
    bool is_visible();
    void set_visibility(bool new_val);
    std::string get_name();
    void set_style(TextStyle style);

protected:
    void _init();
    void _update(const UpdateState& us);
    void onClick(Event* ev);

    spSprite _body;
    spTextField _label;
    std::string name;
    std::string activity_type;
    bool visible = false;
    bool visible_text = false;
};

