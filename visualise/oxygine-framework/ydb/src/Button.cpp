#include "ActorVisualise.h"
#include "Button.h"
#include <algorithm>

void NavigationButton::move(int x, int y) {
    _view->setPosition(x, y);
}

void NavigationButton::_init() {
    _view->setPosition(_scene->getSize() / 2);

    _body = new Sprite;
    _body->setSize(50, 50);
    _body->attachTo(_view);

    _body->addEventListener(TouchEvent::CLICK, CLOSURE(this, &NavigationButton::onClick));
}

void NavigationButton::onClick(Event* ev) {
    TouchEvent* te = safeCast<TouchEvent*>(ev);
    if (te->type == TouchEvent::CLICK)
    {
        state = !state;
    }
}

void NavigationButton::_update(const UpdateState& us)
{
}

void NavigationButton::bind_pictures(ResAnim* main_anim, ResAnim* other_anim) {
    this->main_anim = main_anim;
    this->other_anim = other_anim;
    _body->setResAnim(main_anim);
    _body->setSize(50, 50);
}

bool NavigationButton::get_state() {
    return state;
}

void NavigationButton::reset() {
    state = false;
}

void NavigationButton::switch_picture() {
    std::swap(main_anim, other_anim);
    _body->setResAnim(main_anim);
    _body->setSize(50, 50);
}