#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <Managers/Component.h>
#include <Managers/Properties.h>
#include <Managers/Window.h>
#include <Systems/Move.h>
#include <Systems/Render.h>
#include <Systems/Swarm.h>
#include <Systems/UI.h>

namespace Managers {

class System
{
private:
    Systems::Move move;
    Systems::Render render;
    Systems::Swarm swarm;
    Systems::UI ui;

    float deltaSum    = 0;
    float deltaTime   = 0;
    float fps         = 0;
    float fpsDeltaSum = 0;
    float lastFrame   = 0;

    unsigned int frameCount = 0;

public:
    System();
    ~System();

    void initSystems(GLFWwindow* window);

    void update(
        Managers::Window* windowManager,
        Managers::Component* componentsManager,
        Managers::Properties* propertiesManager
    );
};

}

#endif
