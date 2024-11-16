#pragma once
#include "oxygine-framework.h"
#include "LogParser.h"
#include <vector>
#include <map>
#include <string>
using namespace oxygine;

struct ActorInfo {
    std::vector<int> pos;
    std::pair<float, float> coords;
    ActorInfo();
    ActorInfo(std::vector<int> _pos, std::pair<float, float> _coords);
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

    float time = 0;
    const float time_limit = 300;
    int current_index = 0;
    std::map<std::string, ActorInfo> actors_info;

    std::vector<spActorModel> _actors;
    spArrow _arrow;
};