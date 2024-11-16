#include "ActorModel.h"
#include "ActorVisualise.h"
#include <iostream>


void ActorModel::_init()
{
    _view->setPosition(_scene->getSize() / 2);

    _body = new ColorRectSprite;
    _body->setSize(100, 100);
    _body->setColor(Color(255, 0, 0));
    _body->attachTo(_view);

}

void ActorModel::move(int x, int y) {
    _view->setPosition(x, y);
    // std::cout << x << ' ' << y << std::endl;
}

void ActorModel::resize(int x, int y) {
    _body->setSize(x, y);
}

void ActorModel::activate() {
    _body->setColor(Color(0, 255, 0));
}

void ActorModel::deactivate() {
    _body->setColor(Color(255, 0, 0));
}

void ActorModel::disable() {
    _body->setVisible(false);
}

void ActorModel::enable() {
    _body->setVisible(true);
}

void ActorModel::rename(std::string new_name) {
    name = new_name;
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