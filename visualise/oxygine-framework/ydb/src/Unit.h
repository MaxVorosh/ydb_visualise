#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

class ActorVisualise;
class Unit: public Object
{
public:
    Unit();

    void init(ActorVisualise* scene);

    void update(const UpdateState& us);

protected:
    virtual void _init() {}
    virtual void _update(const UpdateState& us) {}

    spActor _view;
    ActorVisualise* _scene;
};