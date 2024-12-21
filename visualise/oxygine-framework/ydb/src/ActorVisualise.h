#pragma once
#include "oxygine-framework.h"
#include "LogParser.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
using namespace oxygine;


class ActorVisualise;

class VisualiseStageProcessor: public StageInfoVisitor {
public:
    VisualiseStageProcessor() = default;
    VisualiseStageProcessor(ActorVisualise* scene);
    void visit(NewStageInfo* stage) override;
    void visit(RegisterStageInfo* stage) override;
    void visit(SendStageInfo* stage) override;
private:
    ActorVisualise* scene;
};

class VisualiseStageUndoer: public StageInfoVisitor {
public:
    VisualiseStageUndoer() = default;
    VisualiseStageUndoer(ActorVisualise* scene);
    void visit(NewStageInfo* stage) override;
    void visit(RegisterStageInfo* stage) override;
    void visit(SendStageInfo* stage) override;
private:
    ActorVisualise* scene;
};

struct ActorInfo {
    int pos;
    std::pair<float, float> coords;
    std::vector<std::pair<int, std::string>> messages;
    ActorInfo();
    ActorInfo(int _pos, std::pair<float, float> _coords);
};

DECLARE_SMART(FramedProgressBar, spFramedProgressBar);
DECLARE_SMART(ArrowPart, spArrowPart);
DECLARE_SMART(Arrow, spArrow);
DECLARE_SMART(MainArrow, spMainArrow);
DECLARE_SMART(ActorModel, spActorModel);
DECLARE_SMART(ActorVisualise, spActorVisualise);
DECLARE_SMART(NavigationButton, spNavigationButton);
class ActorVisualise: public Actor
{
    friend class VisualiseStageProcessor;
    friend class VisualiseStageUndoer;
public:
    ActorVisualise();
    ~ActorVisualise();

    void init(std::string log_filename);

private:
    VisualiseStageProcessor processor;
    VisualiseStageUndoer undoer;
    LogParser* parser;
    friend class ActorModel;
    friend class Arrow;
    void doUpdate(const UpdateState& us);
    void process_stage();
    void undo_stage();
    spActorModel get_actor(ActorInfo info);
    spActorModel get_actor(std::string actor);
    std::pair<float, float> get_actor_coords(std::string actor);
    bool is_actor_valid(std::string actor);
    void add_new_actor(std::string name, std::string activity_type, std::string type, int x, int y, int size);
    void onEvent(Event* ev);
    void draw_arrow(spArrow moving_arrow, std::string from_actor, std::string to_actor);
    void move(float dx, float dy);
    void update_scale();
    bool should_color();

    float time = 0;
    const float time_limit = 300;
    const int group_size = 8;
    int current_index = 0;
    std::unordered_map<std::string, ActorInfo> actors_info;

    std::vector<spActorModel> _actors;
    spArrow _arrow;
    spArrow _helper_arrow;
    spMainArrow _main_arrow;
    spNavigationButton pause_button;
    spNavigationButton forward_button;
    spNavigationButton backward_button;
    bool on_pause_button = false;
    bool differ = false;
    int lock_stage = -1;

    bool on_pause = false;
    bool on_reverse = false;
    spFramedProgressBar progress_bar;
    const int actor_size = 50;
    std::string point_actor_from;
    std::string point_actor_to;
    std::string main_point_actor_to;
    std::string main_point_actor_from;
    bool is_pointed;
    float scale = 1.0;

    TextStyle style;
    Resources _resources;
};