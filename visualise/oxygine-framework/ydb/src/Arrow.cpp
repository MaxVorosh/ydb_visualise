#include "Arrow.h"
#include "ActorVisualise.h"
#include <iostream>

void ArrowPart::_init()
{
    _view->setPosition(_scene->getSize() / 2);

    _click_box = new ColorRectSprite;
    _click_box->setSize(22, 50);
    _click_box->setColor(Color(0, 0, 0, 0));
    _click_box->attachTo(_view);
    _click_box->setPosition(-10, 0);

    _body = new ColorRectSprite;
    _body->setSize(2, 50);
    _body->setColor(Color(0, 0, 255));
    _body->attachTo(_view);
}

void ArrowPart::disable() {
    _body->setVisible(false);
    _click_box->setVisible(false);
}

void ArrowPart::enable() {
    _body->setVisible(true);
    _click_box->setVisible(true);
}

void ArrowPart::move(float x, float y) {
    this->x = x;
    this->y = y;
    _view->setPosition(x * scale, y * scale);
}

void ArrowPart::rotate(float angle) {
    _view->setRotation(angle);
}

void ArrowPart::scaleY(float new_x) {
    size_x = new_x;
    _view->setScaleY(new_x / sqrt(2504) * scale);
}

void ArrowPart::set_scale(float scale) {
    this->scale = scale;
    scaleY(size_x);
    move(x, y);
}

void ArrowPart::set_color(Color c) {
    _body->setColor(c);
}

void ArrowPart::_update(const UpdateState& us)
{
}

void Arrow::init(ActorVisualise* scene) {
    _body = new ArrowPart;
    _body->init(scene);
    _body->_click_box->addEventListener(TouchEvent::CLICK, CLOSURE(this, &Arrow::onClick));

    _left_part = new ArrowPart;
    _left_part->init(scene);
    _left_part->_click_box->addEventListener(TouchEvent::CLICK, CLOSURE(this, &Arrow::onClick));

    _right_part = new ArrowPart;
    _right_part->init(scene);
    _right_part->_click_box->addEventListener(TouchEvent::CLICK, CLOSURE(this, &Arrow::onClick));

    _label = new TextField;
    _label->setSize(100, 10);
    _label->attachTo(scene);

    _body->disable();
    _left_part->disable();
    _right_part->disable();
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

    label_x = (from_x + to_x) / 2;
    label_y = (from_y + to_y) / 2;
    _label->setPosition(label_x * scale, label_y * scale);
}

void Arrow::enable() {
    _body->enable();
    _left_part->enable();
    _right_part->enable();
    _label->setVisible(true);
}

void Arrow::disable() {
    _body->disable();
    _left_part->disable();
    _right_part->disable();
    _label->setVisible(false);
}

void Arrow::set_type(std::string type) {
    _label->setText(type);
}

void Arrow::set_style(TextStyle style) {
    _label->setStyle(style);
}

void Arrow::set_scale(float scale) {
    this->scale = scale;
    _body->set_scale(scale);
    _left_part->set_scale(scale);
    _right_part->set_scale(scale);
    _label->setPosition(label_x * scale, label_y * scale);
}

void Arrow::set_color(Color c) {
    _body->set_color(c);
    _right_part->set_color(c);
    _left_part->set_color(c);
}

void Arrow::onClick(Event* ev) {}

void MainArrow::onClick(Event* ev)
{
    TouchEvent* te = safeCast<TouchEvent*>(ev);
    if (te->type == TouchEvent::CLICK)
    {
        locked = !locked;
        if (locked) {
            set_color(Color(42, 234, 231));
        }
        else {
            set_color(Color(0, 0, 255));
        }
    }
}

bool MainArrow::is_locked() {
    return locked;
}