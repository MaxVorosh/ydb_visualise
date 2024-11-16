#include "ActorVisualise.h"
#include "ActorModel.h"
#include "Arrow.h"
#include "LogParser.h"
#include <string>
#include <vector>
#include <iostream>
#include <map>

ActorInfo::ActorInfo() {
    group = 0;
    layer = -1;
}

ActorInfo::ActorInfo(int _pos, std::pair<float, float> _coords, int _group, int _layer) {
    pos = _pos;
    coords = _coords;
    group = _group;
    layer = _layer;
}

ActorVisualise::ActorVisualise() {
}

void ActorVisualise::add_new_actor(std::vector<spActorModel>& group, std::string name, int cell_x, int cell_y, int group_number, int layer_number) {
    int current_actors = group.size();
    group.push_back(new ActorModel);
    group.back()->init(this);
    group.back()->rename(name);
    group.back()->resize(cell_x / 5, cell_y / 5);
    int cell_number = (current_actors < 4) ? current_actors : current_actors + 1;
    int column = cell_number % 3;
    int row = cell_number / 3;
    int x_coord = cell_x * column + cell_x / 4 + cell_x / 4 * (row != 1 || column != 0) + cell_x / 4 * (row == 1  && column == 2);
    int y_coord = cell_y * row + cell_y / 2 - cell_y / 8 * (row != 0 || column != 1) + cell_y / 8 * (row == 2 && column == 1);
    group.back()->move(x_coord, y_coord);
    group.back()->disable();
    actors_info[name] = ActorInfo(current_actors, {x_coord + cell_x / 10, y_coord + cell_y / 10}, group_number, layer_number);
}

void ActorVisualise::init(std::string log_filename)
{
    _arrow = new Arrow;
    _arrow->init(this);
    _arrow->disable();
    parser.parse(log_filename);
    // std::cout << "Parsed. Find " << parser.getActors().size() << " actors!" << std::endl;
    //scene layer would have size of display
    auto stage_size = getStage()->getSize();
    setSize(stage_size);
    int group_size = 8;
    int cell_x = stage_size.x / 3;
    int cell_y = stage_size.y / 3;

    std::vector<spActorModel> group;
    std::vector<std::vector<spActorModel>> layer;

    for (auto actor_name: parser.getActors()) {
        add_new_actor(group, actor_name, cell_x, cell_y, layer.size(), 0);
        if (group.size() == group_size) {
            layer.push_back(group);
            group.clear();
            group.reserve(group_size);
        }
    }
    if (group.size() != 0) {
        layer.push_back(group);
    }

    _actors.push_back(layer);
    while (_actors.back().size() != 1) {
        layer.clear();
        group.clear();
        group.reserve(group_size);
        int len = _actors.back().size();

        for (int i = 0; i < len; ++i) {
            std::string actor_name = "layer " + std::to_string(_actors.size() - 1) + ", group " + std::to_string(i);
            add_new_actor(group, actor_name , cell_x, cell_y, layer.size(), _actors.size());
            if (group.size() == group_size) {
                layer.push_back(group);
                group.clear();
                group.reserve(group_size);
            }
        }
        if (group.size() != 0) {
            layer.push_back(group);
        }
        _actors.push_back(layer);
    }
}

bool ActorVisualise::is_actor_valid(std::string actor) {
    return (actors_info.find(actor) != actors_info.end());
}

spActorModel ActorVisualise::get_actor(std::string actor) {
    ActorInfo info = actors_info[actor];
    return _actors[info.layer][info.group][info.pos];
}

std::pair<float, float> ActorVisualise::get_actor_coords(std::string actor) {
    if (on_screen(actor)) {
        return actors_info[actor].coords;
    }
    auto stage_size = getStage()->getSize();
    return {stage_size.x / 2, stage_size.y};
}

bool ActorVisualise::on_screen(std::string actor) {
    return (actors_info[actor].group == current_group && actors_info[actor].layer == current_layer);
}

void ActorVisualise::process_stage() {
    _arrow->disable();
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        if (on_screen(stage.main_actor)) {
            main_actor->enable();
        }
    }
    else if (stage.type == StageType::Register) {
        if (on_screen(stage.main_actor)) {
            main_actor->activate();
        }
    }
    else if (stage.type == StageType::Send) {
        if (!is_actor_valid(stage.other_actor)) {
            return;
        }
        // std::cout << "Send " << stage.main_actor << ' ' << stage.other_actor << std::endl;
        _arrow->enable();
        auto from_coords = get_actor_coords(stage.main_actor);
        auto to_coords = get_actor_coords(stage.other_actor);
        _arrow->point(from_coords.first, from_coords.second, to_coords.first, to_coords.second);
    }
}

void ActorVisualise::doUpdate(const UpdateState& us)
{
    //doUpdate is virtual method of Actor
    //it is being called each frame

    //update player each frame
    // std::cout << time << ' ' << us.dt << std::endl;
    time += us.dt;
    if (time > time_limit) {
        time = 0;
        if (current_index < parser.getStages().size()) {
            process_stage();
            current_index++;
        }
    }
}