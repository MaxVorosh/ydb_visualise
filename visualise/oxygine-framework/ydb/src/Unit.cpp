#include "Unit.h"
#include "ActorVisualise.h"

Unit::Unit(): _scene(0)
{

}

void Unit::init(ActorVisualise* scene)
{
    //initialize base
    _scene = scene;
    _view = new Actor;
    _view->attachTo(scene);

    _init();
}

void Unit::update(const UpdateState& us)
{
    _update(us);
}