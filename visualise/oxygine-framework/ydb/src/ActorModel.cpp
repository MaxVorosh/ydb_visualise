#include "ActorModel.h"
#include "ActorVisualise.h"
#include <iostream>


void ActorModel::_init()
{
    _view->setPosition(_scene->getSize() / 2);

    _body = new ColorRectSprite;
    _body->setSize(base_size_x, base_size_y);
    _body->setColor(Color(255, 0, 0));
    _body->attachTo(_view);

    _body->addEventListener(TouchEvent::CLICK, CLOSURE(this, &ActorModel::onClick));

    _label = new TextField;
    _label->setSize(100, 10);
    _label->attachTo(_view);
}

void ActorModel::move(int x, int y) {
    this->x = x;
    this->y = y;
    _view->setPosition(x * scale, y * scale);
    auto body_size = _body->getSize();
    _label->setPosition(0, body_size.y);
    // std::cout << x << ' ' << y << ' ' << body_size.x << ' ' << body_size.y << std::endl;
}

void ActorModel::resize(int x, int y) {
    _body->setSize(x * scale, y * scale);
    base_size_x = x;
    base_size_y = y;
}

void ActorModel::activate() {
    _body->setColor(Color(0, 255, 0));
}

void ActorModel::deactivate() {
    _body->setColor(Color(255, 0, 0));
}

void ActorModel::disable() {
    _body->setVisible(false);
    _label->setVisible(false);
}

void ActorModel::enable() {
    _body->setVisible(true);
}

void ActorModel::rename(std::string new_name) {
    name = new_name;
    update_label();
}

void ActorModel::set_activity_type(std::string new_activity_type) {
    activity_type = new_activity_type;
    update_label();
}

void ActorModel::set_type(std::string new_type) {
    type = new_type;
    update_label();
}

void ActorModel::update_label() {
    _label->setText(name + '\n' + activity_type + '\n' + type);
}

void ActorModel::_update(const UpdateState& us)
{
}

bool ActorModel::is_visible() {
    return visible;
}

void ActorModel::set_visibility(bool new_val) {
    visible = new_val;
}

std::string ActorModel::get_name() {
    return name;
}

void ActorModel::set_style(TextStyle style) {
    _label->setStyle(style);
}

void ActorModel::set_scale(float scale) {
    this->scale = scale;
    resize(base_size_x, base_size_y);
    move(x, y);
}

std::pair<int, int> ActorModel::get_center_position() {
    return {x + base_size_x / 2, y + base_size_y / 2};
}

void ActorModel::onClick(Event* ev)
{
    TouchEvent* te = safeCast<TouchEvent*>(ev);
    if (te->type == TouchEvent::CLICK)
    {
        visible_text = !visible_text;
        _label->setVisible(visible_text);
    }
}

void ActorGroup::_init() {
    _view->setPosition(_scene->getSize() / 2);

    frame_bg = new ColorRectSprite;
    frame_bg->setSize(base_size_x + 10, base_size_y + 10);
    frame_bg->setColor(Color(255, 255, 255));
    frame_bg->attachTo(_view);

    bg = new ColorRectSprite;
    bg->setSize(base_size_x + 6, base_size_y + 6);
    bg->setColor(Color(0, 0, 0));
    bg->attachTo(frame_bg);
    bg->setPosition(2, 2);
    
    _label = new TextField;
    _label->setSize(100, 10);
    _label->attachTo(_view);
    _label->setPosition(10, base_size_y + 6);
}

void ActorGroup::_update(const UpdateState& us) {}

void ActorGroup::move(int x, int y) {
    this->x = x;
    this->y = y;
    frame_bg->setPosition(x, y);
    _label->setPosition(x, y + base_size_y * scale + 6);
}

void ActorGroup::set_node_id(std::string node_id) {
    this->node_id = node_id;
    _label->setText("Node: " + node_id);
}

void ActorGroup::add_actor(spActorModel actor) {
    actors.push_back(actor);
}

std::pair<int, int> ActorGroup::get_base_size() {
    int horizontal_cnt = sqrt(actors.size());
    int vertical_cnt = actors.size() / horizontal_cnt;
    if (vertical_cnt * horizontal_cnt != actors.size()) {
        vertical_cnt++;
    }
    return {horizontal_cnt * 100 - 50,  vertical_cnt * 100 - 50};
}

void ActorGroup::show() {
    auto size = get_base_size();
    base_size_x = size.first;
    base_size_y = size.second;
    frame_bg->setSize(size.first + 10, size.second + 10);
    bg->setSize(size.first + 6, size.second + 6);
    _label->setPosition(x, y + size.second + 6);

    
}

void ActorGroup::attach_actors() {
    int horizontal_cnt = sqrt(actors.size());
    for (int i = 0; i < actors.size(); ++i) {
        auto actor = actors[i];
        int column = i % horizontal_cnt;
        int row = i / horizontal_cnt;
        actor->move(x + 4 + column * 100, y + 4 + row * 100);
        std::cout << x + 4 + column * 100 << ' ' << y + 4 + row * 100 << std::endl;
    }
}

void ActorGroup::set_scale(float scale) {
    this->scale = scale;
    frame_bg->setSize(scale * base_size_x + 10, scale * base_size_y + 10);
    bg->setSize(scale * base_size_x + 6, scale * base_size_y + 6);
    move(x, y);
}

void ActorGroup::set_style(TextStyle style) {
    this->style = style;
    _label->setStyle(style);
}

std::vector<spActorModel>* ActorGroup::get_actors() {
    return &actors;
}