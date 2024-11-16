#include "Arrow.h"
#include "ActorVisualise.h"
#include <iostream>

void ArrowPart::_init()
{
    _view->setPosition(_scene->getSize() / 2);

    _body = new ColorRectSprite;
    _body->setSize(2, 50);
    _body->setColor(Color(0, 0, 255));
    _body->attachTo(_view);
}

void ArrowPart::disable() {
    _body->setVisible(false);
}

void ArrowPart::enable() {
    _body->setVisible(true);
}

void ArrowPart::move(float x, float y) {
    _view->setPosition(x, y);
}

void ArrowPart::rotate(float angle) {
    _view->setRotation(angle);
}

void ArrowPart::scaleY(float new_x) {
    _view->setScaleY(new_x / sqrt(2504));
}

void ArrowPart::_update(const UpdateState& us)
{
}

void Arrow::init(ActorVisualise* scene) {
    _body = new ArrowPart;
    _body->init(scene);

    _left_part = new ArrowPart;
    _left_part->init(scene);

    _right_part = new ArrowPart;
    _right_part->init(scene);
}

void Arrow::point(int from_x, int from_y, int to_x, int to_y) {
    // std::cout << from_x << ' ' << from_y << ' ' << to_x << ' ' << to_y << std::endl;
    float dx = to_x - from_x;
    float dy = to_y - from_y;
    float angle = atan2(dy, dx) - 3.14 / 2.0;
    // std::cout << angle << std::endl;
    _body->scaleY(sqrt(dx * dx + dy * dy));
    _body->move(from_x, from_y);
    _body->rotate(angle);

    _left_part->scaleY(std::max(sqrt(dx * dx + dy * dy) * 0.1, 50.0));
    _left_part->move(to_x, to_y);
    _left_part->rotate(angle + 3.14 * 5 / 6);

    _right_part->scaleY(std::max(sqrt(dx * dx + dy * dy) * 0.1, 50.0));
    _right_part->move(to_x, to_y);
    _right_part->rotate(angle - 3.14 * 5 / 6);
}

void Arrow::enable() {
    _body->enable();
    _left_part->enable();
    _right_part->enable();
}

void Arrow::disable() {
    _body->disable();
    _left_part->disable();
    _right_part->disable();
}