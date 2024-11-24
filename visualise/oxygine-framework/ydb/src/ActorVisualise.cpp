#include "ActorVisualise.h"
#include "ActorModel.h"
#include "Arrow.h"
#include "LogParser.h"
#include "FramedProgressBar.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

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

ArrowLine::ArrowLine() {}

ArrowLine::ArrowLine(int f_x, int f_y, int t_x, int t_y) {
    from_x = f_x;
    from_y = f_y;
    to_x = t_x;
    to_y = t_y;
}

std::size_t PairHash::operator()(const std::pair<int, int> &p) const noexcept {
    return p.first + 20 * p.second; // Pretty sure, that number of layers will not be more than 20
}

ActorVisualise::ActorVisualise() {
    ox::core::getDispatcher()->addEventListener(ox::core::EVENT_SYSTEM, CLOSURE(this, &ActorVisualise::onEvent));
    _resources.loadXML("res.xml");
}

void ActorVisualise::add_new_actor(std::vector<spActorModel>& group, std::string name, std::string activity_type, int cell_x, int cell_y, int group_number, int layer_number) {
    int current_actors = group.size();
    group.push_back(new ActorModel);
    group.back()->init(this);
    group.back()->rename(name);
    group.back()->set_activity_type(activity_type);
    group.back()->resize(cell_x / 5, cell_y / 5);

    int cell_number = (current_actors < 4) ? current_actors : current_actors + 1;
    int column = cell_number % 3;
    int row = cell_number / 3;
    int x_coord = cell_x * column + cell_x / 4 + cell_x / 4 * (row != 1 || column != 0) + cell_x / 4 * (row == 1  && column == 2);
    int y_coord = cell_y * row + cell_y / 2 - cell_y / 8 * (row != 0 || column != 1) + cell_y / 8 * (row == 2 && column == 1);
    group.back()->move(x_coord, y_coord);

    TextStyle style(_resources.getResFont("main"));
    style.fontSize = 15;
    style.color = Color(255, 255, 255);
    group.back()->set_style(style);

    group.back()->disable();
    actors_info[name] = ActorInfo(current_actors, {x_coord + cell_x / 10, y_coord + cell_y / 10}, group_number, layer_number);
}

void ActorVisualise::init(std::string log_filename)
{
    progress_bar = new FramedProgressBar;
    progress_bar->init(this);

    parser.parse(log_filename);
    // std::cout << "Parsed. Find " << parser.getActors().size() << " actors!" << std::endl;
    //scene layer would have size of display
    auto stage_size = getStage()->getSize();
    setSize(stage_size);
    int cell_x = stage_size.x / 3;
    int cell_y = stage_size.y / 3;

    std::vector<spActorModel> group;
    std::vector<std::vector<spActorModel>> layer;

    for (auto actor_info: parser.getActorsInfo()) {
        add_new_actor(group, actor_info.name, actor_info.activity_type, cell_x, cell_y, layer.size(), 0);
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
            std::string activity_type = "Group Actor";
            add_new_actor(group, actor_name, activity_type, cell_x, cell_y, layer.size(), _actors.size());
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

    _arrow = new Arrow;
    _arrow->init(this);
    _arrow->disable();
    TextStyle style(_resources.getResFont("main"));
    style.fontSize = 15;
    style.color = Color(255, 255, 255);
    _arrow->set_style(style);
}

bool ActorVisualise::is_actor_valid(std::string actor) {
    return (actors_info.find(actor) != actors_info.end());
}

spActorModel ActorVisualise::get_actor(ActorInfo info) {
    return _actors[info.layer][info.group][info.pos];
}

spActorModel ActorVisualise::get_actor(std::string actor) {
    ActorInfo info = actors_info[actor];
    return get_actor(info);
}

std::pair<float, float> ActorVisualise::get_actor_coords(std::string actor) {
    return actors_info[actor].coords;
}

bool ActorVisualise::on_screen(std::string actor) {
    return (actors_info[actor].group == current_group && actors_info[actor].layer == current_layer);
}

void ActorVisualise::compute_arrow(std::string from_actor, std::string to_actor) {
    int from_group = actors_info[from_actor].group;
    int from_layer = actors_info[from_actor].layer;
    int from_pos = actors_info[from_actor].pos;
    int to_group = actors_info[to_actor].group;
    int to_layer = actors_info[to_actor].layer;
    int to_pos = actors_info[to_actor].pos;
    auto from_coord = get_actor_coords(from_actor);
    auto to_coord = get_actor_coords(to_actor);
    auto stage_size = getStage()->getSize();
    while (from_layer < to_layer) {
        arrow_lines[{from_group, from_layer}] = ArrowLine(from_coord.first, from_coord.second, stage_size.x / 2, 0);
        from_coord = {stage_size.x / 2, stage_size.y};
        from_layer++;
        from_pos = from_group % group_size;
        from_group /= group_size;
    }
    while (to_layer < from_layer) {
        arrow_lines[{to_group, to_layer}] = ArrowLine(stage_size.x / 2, 0, to_coord.first, to_coord.second);
        to_coord = {stage_size.x / 2, stage_size.y};
        to_layer++;
        to_pos = to_group % group_size;
        to_group /= group_size;
    }
    while (from_group != to_group) {
        arrow_lines[{from_group, from_layer}] = ArrowLine(from_coord.first, from_coord.second, stage_size.x / 2, 0);
        from_coord = {stage_size.x / 2, stage_size.y};
        from_layer++;
        from_pos = from_group % group_size;
        from_group /= group_size;
        arrow_lines[{to_group, to_layer}] = ArrowLine(stage_size.x / 2, 0, to_coord.first, to_coord.second);
        to_coord = {stage_size.x / 2, stage_size.y};
        to_layer++;
        to_pos = to_group % group_size;
        to_group /= group_size;
    }
    std::string final_from_actor = _actors[from_layer][from_group][from_pos]->get_name();
    std::string final_to_actor = _actors[to_layer][to_group][to_pos]->get_name();
    from_coord = get_actor_coords(final_from_actor);
    to_coord = get_actor_coords(final_to_actor);
    arrow_lines[{from_group, from_layer}] = ArrowLine(from_coord.first, from_coord.second, to_coord.first, to_coord.second);
}

void ActorVisualise::draw_arrow() {
    if (arrow_lines.find({current_group, current_layer}) == arrow_lines.end()) {
        _arrow->disable();
        return;
    }
    auto line = arrow_lines[{current_group, current_layer}];
    _arrow->enable();
    _arrow->point(line.from_x, line.from_y, line.to_x, line.to_y);
}

void ActorVisualise::process_stage() {
    _arrow->disable();
    arrow_lines.clear();
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        main_actor->set_visibility(true);
        if (on_screen(stage.main_actor)) {
            main_actor->enable();
        }
    }
    else if (stage.type == StageType::Register) {
        main_actor->activate();
    }
    else if (stage.type == StageType::Send) {
        if (!is_actor_valid(stage.other_actor)) {
            return;
        }
        compute_arrow(stage.main_actor, stage.other_actor);
        draw_arrow();
        _arrow->set_type(stage.info);
    }
}

void ActorVisualise::undo_stage() {
    _arrow->disable();
    arrow_lines.clear();
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        main_actor->set_visibility(false);
        if (on_screen(stage.main_actor)) {
            main_actor->disable();
        }
    }
    else if (stage.type == StageType::Register) {
        main_actor->deactivate();
    }
    else if (stage.type == StageType::Send) {}
}

void ActorVisualise::doUpdate(const UpdateState& us)
{
    //doUpdate is virtual method of Actor
    //it is being called each frame

    //update player each frame
    // std::cout << time << ' ' << us.dt << std::endl;
    if (!on_pause) {
        time += us.dt;
    }
    if (time > time_limit) {
        time = 0;
        if (on_reverse) {
            if (current_index > 0) {
                current_index--;
                undo_stage();
                if (current_index > 0) {
                    current_index--;
                    process_stage();
                    current_index++;
                }
            }
        }
        else {
            if (current_index < parser.getStages().size()) {
                process_stage();
                current_index++;
            }
        }
    }
    progress_bar->setProgress(current_index / (float)parser.getStages().size());
}

void ActorVisualise::load_new_actors() {
    for (auto actor: _actors[current_layer][current_group]) {
        if (current_layer > 0 || actor->is_visible()) {
            actor->enable();
        }
    }
    draw_arrow();
}

void ActorVisualise::remove_old_actors() {
    for (auto actor: _actors[current_layer][current_group]) {
        actor->disable();
    }
}

void ActorVisualise::change_layer(int new_layer) {
    if (new_layer < 0 || new_layer >= _actors.size()) {
        return;
    }
    remove_old_actors();
    current_layer = new_layer;
    if (new_layer < current_layer) {
        current_group *= group_size;
    }
    else {
        current_group /= group_size;
    }
    load_new_actors();
}

void ActorVisualise::change_group(int new_group) {
//    std::cout << "New group " << new_group << std::endl;
    if (new_group < 0 || new_group >= _actors[current_layer].size()) {
        return;
    }
    remove_old_actors();
    current_group = new_group;
    load_new_actors();
}

void ActorVisualise::onEvent(Event* ev)
{
    SDL_Event *event = (SDL_Event*)ev->userData;

    if (event->type != SDL_KEYDOWN)
        return;

    //all key codes could be found in "SDL_keyboard.h" from SDL
    switch (event->key.keysym.sym)
    {
        case SDLK_r:
            on_reverse = !on_reverse;
            break;
        case SDLK_SPACE:
            on_pause = !on_pause;
            break;
        case SDLK_d:
            change_group(current_group + 1);
            break;
        case SDLK_a:
            change_group(current_group - 1);
            break;
        case SDLK_w:
            change_layer(current_layer + 1);
            break;
        case SDLK_s:
            change_layer(current_layer - 1);
            break;
        case SDLK_RIGHT:
            change_group(current_group + 1);
            break;
        case SDLK_LEFT:
            change_group(current_group - 1);
            break;
        case SDLK_UP:
            change_layer(current_layer + 1);
            break;
        case SDLK_DOWN:
            change_layer(current_layer - 1);
            break;
    }
}