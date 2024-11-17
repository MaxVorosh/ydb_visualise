#include "oxygine-framework.h"
#include "ActorVisualise.h"
#include <iostream>
using namespace oxygine;

void helper_preinit()
{
}

void helper_init()
{

    //create scene with game
    spActorVisualise scene = new ActorVisualise;
    scene->init("logs/log.txt");
    scene->attachTo(getStage());
}

void helper_update()
{

}

void helper_destroy()
{
}