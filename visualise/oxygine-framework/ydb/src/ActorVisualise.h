#pragma once
#include "oxygine-framework.h"
#include "LogParser.h"
#include <vector>
#include <map>
#include <string>
using namespace oxygine;

struct ActorInfo {
    int pos;
    std::pair<float, float> coords;
    int group;
    int layer;
    ActorInfo();
    ActorInfo(int _pos, std::pair<float, float> _coords, int _group, int _layer);
};

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
    spActorModel get_actor(std::string actor);
    std::pair<float, float> get_actor_coords(std::string actor);
    bool is_actor_valid(std::string actor);
    void add_new_actor(std::vector<spActorModel>& group, std::string name, int cell_x, int cell_y, int group_number, int layer_number);
    bool on_screen(std::string actor);

    float time = 0;
    const float time_limit = 300;
    int current_index = 0;
    std::map<std::string, ActorInfo> actors_info;

    std::vector<std::vector<std::vector<spActorModel>>> _actors;
    spArrow _arrow;

    int current_group;
    int current_layer;
};