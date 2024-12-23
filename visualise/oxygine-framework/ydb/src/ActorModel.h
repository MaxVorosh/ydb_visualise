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
    void set_scale(float scale);
    void set_type(std::string new_type);
    std::pair<int, int> get_center_position();

protected:
    void _init();
    void _update(const UpdateState& us);
    void onClick(Event* ev);
    void update_label();

    spSprite _body;
    spTextField _label;
    std::string name;
    std::string activity_type;
    std::string type;
    bool visible = false;
    bool visible_text = false;
    float scale = 1.0;
    float base_size_x = 100.0;
    float base_size_y = 100.0;
    float x;
    float y;
};

DECLARE_SMART(ActorGroup, spActorGroup);
class ActorGroup: public Unit {
public:
    void add_actor(spActorModel actor);
    void move(int x, int y);
    std::pair<int, int> get_base_size();
    void set_node_id(std::string node_id);
    void set_scale(float scale);
    void set_style(TextStyle style);
    void show();
    std::vector<spActorModel>* get_actors();
    void attach_actors();
protected:
    void _init();
    void _update(const UpdateState& us);

    spColorRectSprite frame_bg;
    spColorRectSprite bg;
    spTextField _label;

    std::string node_id;
    std::vector<spActorModel> actors;
    float scale = 1.0;
    int base_size_x;
    int base_size_y;
    int x;
    int y;
    TextStyle style;
};