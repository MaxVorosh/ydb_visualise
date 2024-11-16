#include "ActorVisualise.h"
#include "ActorModel.h"
#include "Arrow.h"
#include "LogParser.h"
#include <string>
#include <vector>
#include <iostream>
#include <map>

ActorInfo::ActorInfo() {}

ActorInfo::ActorInfo(std::vector<int> _pos, std::pair<float, float> _coords) {
    pos = _pos;
    coords = _coords;
}

ActorVisualise::ActorVisualise() {
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
    int current_actors = 0;
    int cell_x = stage_size.x / 3;
    int cell_y = stage_size.y / 3;

    for (auto actor_name: parser.getActors()) {
        _actors.push_back(new ActorModel);
        _actors.back()->init(this);
        _actors.back()->rename(actor_name);
        _actors.back()->resize(cell_x / 5, cell_y / 5);
        int cell_number = (current_actors < 4) ? current_actors : current_actors + 1;
        int column = cell_number % 3;
        int row = cell_number / 3;
        int x_coord = cell_x * column + cell_x / 4 + cell_x / 4 * (row != 1 || column != 0) + cell_x / 4 * (row == 1  && column == 2);
        int y_coord = cell_y * row + cell_y / 4 + cell_y / 4 * (row != 0 || column != 1) + cell_y / 4 * (row == 2 && column == 1);
        _actors.back()->move(x_coord, y_coord);
        _actors.back()->disable();
        actors_info[actor_name] = ActorInfo({current_actors}, {x_coord + cell_x / 10, y_coord + cell_y / 10});

        current_actors++;
        if (current_actors == group_size) {
            current_actors = 0;
            break;
        }
    }
}

bool ActorVisualise::is_actor_valid(std::string actor) {
    return (actors_info.find(actor) != actors_info.end());
}

spActorModel ActorVisualise::get_actor(std::string actor) {
    std::vector<int> pos = actors_info[actor].pos;
    return _actors[pos.back()];
}

std::pair<float, float> ActorVisualise::get_actor_coords(std::string actor) {
    return actors_info[actor].coords;
}

void ActorVisualise::process_stage() {
    _arrow->disable();
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        main_actor->enable();
    }
    else if (stage.type == StageType::Register) {
        main_actor->activate();
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