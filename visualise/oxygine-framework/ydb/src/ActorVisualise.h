#pragma once
#include "oxygine-framework.h"
#include "LogParser.h"
#include <vector>
#include <unordered_map>
#include <string>
using namespace oxygine;

struct ActorInfo {
    int pos;
    std::pair<float, float> coords;
    ActorInfo();
    ActorInfo(int _pos, std::pair<float, float> _coords);
};

DECLARE_SMART(FramedProgressBar, spFramedProgressBar);
DECLARE_SMART(ArrowPart, spArrowPart);
DECLARE_SMART(Arrow, spArrow);
DECLARE_SMART(ActorModel, spActorModel);
DECLARE_SMART(ActorVisualise, spActorVisualise);
class ActorVisualise: public Actor
{
public:
    ActorVisualise();

    void init(std::string log_filename);

private:
    LogParser parser;
    friend class ActorModel;
    friend class Arrow;
    void doUpdate(const UpdateState& us);
    void process_stage();
    void undo_stage();
    spActorModel get_actor(ActorInfo info);
    spActorModel get_actor(std::string actor);
    std::pair<float, float> get_actor_coords(std::string actor);
    bool is_actor_valid(std::string actor);
    void add_new_actor(std::string name, std::string activity_type, int x, int y, int size);
    void onEvent(Event* ev);
    void draw_arrow(std::string from_actor, std::string to_actor);
    void move(float dx, float dy);
    void update_scale();

    float time = 0;
    const float time_limit = 300;
    const int group_size = 8;
    int current_index = 0;
    std::unordered_map<std::string, ActorInfo> actors_info;

    std::vector<spActorModel> _actors;
    spArrow _arrow;

    bool on_pause = false;
    bool on_reverse = false;
    spFramedProgressBar progress_bar;
    const int actor_size = 50;
    std::string point_actor_from;
    std::string point_actor_to;
    bool is_pointed;
    float scale = 1.0;

    Resources _resources;
};