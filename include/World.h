#ifndef WORLD_H
#define WORLD_H

#include <Managers/Component.h>
#include <Managers/Entity.h>
#include <Managers/Properties.h>
#include <Managers/System.h>
#include <Managers/Window.h>

struct World
{
    Managers::Window windowManager;
    Managers::Entity entityManager;
    Managers::Component componentManager;
    Managers::Properties propertiesManager;
    Managers::System systemManager;
    // systems[];
};

#endif
