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
    bool is_visible();
    void set_visibility(bool new_val);
    std::string get_name();

protected:
    void _init();
    void _update(const UpdateState& us);

    spSprite _body;
    std::string name;
    bool visible = false;
};

