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
    int group;
    int layer;
    ActorInfo();
    ActorInfo(int _pos, std::pair<float, float> _coords, int _group, int _layer);
};

struct ArrowLine {
    int from_x;
    int from_y;
    int to_x;
    int to_y;
    ArrowLine();
    ArrowLine(int f_x, int f_y, int t_x, int t_y);
};

struct PairHash {
    std::size_t operator()(const std::pair<int, int> &p) const noexcept;
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
    void add_new_actor(std::vector<spActorModel>& group, std::string name, int cell_x, int cell_y, int group_number, int layer_number);
    bool on_screen(std::string actor);
    void onEvent(Event* ev);
    void load_new_actors();
    void remove_old_actors();
    void change_layer(int new_layer);
    void change_group(int new_group);
    void compute_arrow(std::string from_actor, std::string to_actor);
    void draw_arrow();

    float time = 0;
    const float time_limit = 300;
    const int group_size = 8;
    int current_index = 0;
    std::unordered_map<std::string, ActorInfo> actors_info;

    std::vector<std::vector<std::vector<spActorModel>>> _actors;
    spArrow _arrow;
    std::unordered_map<std::pair<int, int>, ArrowLine, PairHash> arrow_lines;

    int current_group;
    int current_layer;
    bool on_pause = false;
    bool on_reverse = false;
    spFramedProgressBar progress_bar;

    Resources _resources;
};