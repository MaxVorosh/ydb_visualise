#include "ActorVisualise.h"
#include "ActorModel.h"
#include "Arrow.h"
#include "LogParser.h"
#include "FramedProgressBar.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <queue>

int find_by_timestamp(int timestamp, std::vector<std::pair<int, std::string>>& data) {
    // Binary search
    int l = -1;
    int r = data.size();
    while (r - l > 1) {
        int m = (r + l) / 2;
        if (data[m].first <= timestamp) {
            l = m;
        }
        else {
            r = m;
        }
    }
    return r;
}

ActorInfo::ActorInfo() {
}

ActorInfo::ActorInfo(int _pos, std::pair<float, float> _coords) {
    pos = _pos;
    coords = _coords;
}

ActorVisualise::ActorVisualise() {
    ox::core::getDispatcher()->addEventListener(ox::core::EVENT_SYSTEM, CLOSURE(this, &ActorVisualise::onEvent));
    _resources.loadXML("res.xml");
}

void ActorVisualise::add_new_actor(std::string name, std::string activity_type, std::string type, int x, int y, int size) {
    int current_actors = _actors.size();
    _actors.push_back(new ActorModel);
    _actors.back()->init(this);
    _actors.back()->rename(name);
    _actors.back()->set_activity_type(activity_type);
    _actors.back()->set_type(type);
    _actors.back()->resize(size, size);
    _actors.back()->move(x, y);

    TextStyle style(_resources.getResFont("main"));
    style.fontSize = 15;
    style.color = Color(255, 255, 255);
    _actors.back()->set_style(style);

    _actors.back()->disable();
    actors_info[name] = ActorInfo(current_actors, {x + size / 2, y + size / 2});
}

void ActorVisualise::init(std::string log_filename)
{
    parser.parse(log_filename);
    int side = sqrt(parser.getActorsInfo().size());
    auto size = getStage()->getSize();
    int space = 50;
    int stride = actor_size + space;
    int min_x = 10;
    int min_y = 10;
    int row = 0;
    int column = 0;

    for (auto actor_info: parser.getActorsInfo()) {
        int x = min_x + stride * column;
        int y = min_y + stride * row;
        add_new_actor(actor_info.name, actor_info.activity_type, actor_info.type, x, y, actor_size);
        column++;
        if (column == side) {
            column = 0;
            row++;
        }
    }

    style = TextStyle(_resources.getResFont("main"));
    style.fontSize = 15;
    style.color = Color(255, 255, 255);

    _main_arrow = new MainArrow;
    _main_arrow->init(this);
    _main_arrow->set_style(style);
    _main_arrow->disable();

    _helper_arrow = new Arrow;
    _helper_arrow->init(this);
    _helper_arrow->set_style(style);
    _helper_arrow->disable();

    _arrow = _main_arrow;

    progress_bar = new FramedProgressBar;
    progress_bar->init(this);
}

bool ActorVisualise::is_actor_valid(std::string actor) {
    return (actors_info.find(actor) != actors_info.end());
}

spActorModel ActorVisualise::get_actor(ActorInfo info) {
    return _actors[info.pos];
}

spActorModel ActorVisualise::get_actor(std::string actor) {
    ActorInfo info = actors_info[actor];
    return get_actor(info);
}

std::pair<float, float> ActorVisualise::get_actor_coords(std::string actor) {
    return actors_info[actor].coords;
}

void ActorVisualise::draw_arrow(spArrow moving_arrow, std::string from_actor, std::string to_actor) {
    if (!is_actor_valid(from_actor) || !is_actor_valid(to_actor)) {
        moving_arrow->disable();
        return;
    }
    moving_arrow->enable();
    auto from_coords = get_actor_coords(from_actor);
    auto to_coords = get_actor_coords(to_actor);
    moving_arrow->point(from_coords.first, from_coords.second, to_coords.first, to_coords.second);
}

bool ActorVisualise::should_color() {
    std::string& actor = main_point_actor_to;
    int last_time = lock_stage;
    // std::cout << "Start " << current_index << ' ' << actor << ' ' << last_time << std::endl;
    int index = find_by_timestamp(lock_stage, actors_info[actor].messages);
    while (index < actors_info[actor].messages.size()) {
        last_time = actors_info[actor].messages[index].first;
        actor = actors_info[actor].messages[index].second;
        // std::cout << "Start " << actor << ' ' << last_time << std::endl;
        index = find_by_timestamp(lock_stage, actors_info[actor].messages);
    }
    return (last_time == current_index);
}

void ActorVisualise::process_stage() {
    if (_main_arrow->is_locked()) {
        if (!differ) {
            _arrow = _helper_arrow;
            differ = true;
            lock_stage = current_index;
            main_point_actor_to = point_actor_to;
            main_point_actor_from = point_actor_from;
        }
    }
    else if (differ) {
        differ = false;
        _arrow = _main_arrow;
        _helper_arrow->disable();
        lock_stage = -1;
    }
    _arrow->disable();
    is_pointed = false;
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        main_actor->set_visibility(true);
        main_actor->enable();
    }
    else if (stage.type == StageType::Register) {
        main_actor->activate();
    }
    else if (stage.type == StageType::Send) {
        if (!is_actor_valid(stage.other_actor)) {
            return;
        }
        if (actors_info[stage.main_actor].messages.empty() ||
             current_index > actors_info[stage.main_actor].messages.back().first) {
            actors_info[stage.main_actor].messages.push_back({current_index, stage.other_actor});
        }
        draw_arrow(_arrow, stage.main_actor, stage.other_actor);
        _arrow->set_type(stage.info);
        is_pointed = true;
        point_actor_from = stage.main_actor;
        point_actor_to = stage.other_actor;
        if (_main_arrow->is_locked() && should_color()) {
            _arrow->set_color(Color(240, 230, 140));
        }
        else {
            _arrow->set_color(Color(0, 0, 255));
        }
    }
}

void ActorVisualise::undo_stage() {
    if (lock_stage == current_index) {
        _arrow = _main_arrow;
        _helper_arrow->disable();
        lock_stage = -1;
        _main_arrow->switch_lock();
    }
    _arrow->disable();
    is_pointed = false;
    StageInfo stage = parser.getStages()[current_index];
    if (!is_actor_valid(stage.main_actor)) {
        return;
    }
    spActorModel main_actor = get_actor(stage.main_actor);
    if (stage.type == StageType::New) {
        main_actor->set_visibility(false);
        main_actor->disable();
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

void ActorVisualise::move(float dx, float dy) {
    for (auto actor_name: parser.getActors()) {
        auto actor = get_actor(actor_name);
        auto info = actors_info[actor_name];
        auto coords = info.coords;
        actor->move(coords.first + dx - actor_size / 2, coords.second + dy - actor_size / 2);
        actors_info[actor_name].coords.first += dx;
        actors_info[actor_name].coords.second += dy;
    }
    if (is_pointed) {
        draw_arrow(_arrow, point_actor_from, point_actor_to);
        if (differ) {
            draw_arrow(_main_arrow, main_point_actor_from, main_point_actor_to);
        }
    }
}

void ActorVisualise::update_scale() {
    for (auto actor: _actors) {
        actor->set_scale(scale);
    }
    _arrow->set_scale(scale);
    _main_arrow->set_scale(scale);
}

void ActorVisualise::onEvent(Event* ev)
{
    SDL_Event *event = (SDL_Event*)ev->userData;
    float v_x = 10;
    float v_y = 10;
    float v_scale = 0.9;

    if (event->type == SDL_MOUSEWHEEL) {
        if (event->wheel.y == 1) {
            scale /= v_scale;
        }
        else {
            scale *= v_scale;
        }
        update_scale();
        return;
    }

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
            move(v_x, 0);
            break;
        case SDLK_a:
            move(-v_x, 0);
            break;
        case SDLK_w:
            move(0, -v_y);
            break;
        case SDLK_s:
            move(0, v_y);
            break;
        case SDLK_RIGHT:
            move(v_x, 0);
            break;
        case SDLK_LEFT:
            move(-v_x, 0);
            break;
        case SDLK_UP:
            move(0, -v_y);
            break;
        case SDLK_DOWN:
            move(0, v_y);
            break;
    }
}