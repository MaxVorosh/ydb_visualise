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
    _label->setPosition(0, body_size.y * scale);
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
    _label->setText(name + '\n' + activity_type);
}

void ActorModel::set_activity_type(std::string new_activity_type) {
    activity_type = new_activity_type;
    _label->setText(name + '\n' + activity_type);
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

void ActorModel::onClick(Event* ev)
{
    TouchEvent* te = safeCast<TouchEvent*>(ev);
    if (te->type == TouchEvent::CLICK)
    {
        visible_text = !visible_text;
        _label->setVisible(visible_text);
    }
}