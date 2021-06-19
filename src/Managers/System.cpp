#include <Managers/System.h>

namespace Managers {

System::System() {}

System::~System() {}

void System::initSystems(GLFWwindow* window) {
    ui.init(window);
}

void System::update(
    Managers::Window* windowManager,
    Managers::Component* componentsManager,
    Managers::Properties* propertiesManager
) {
    float currentFrame = glfwGetTime();

    deltaTime    = currentFrame - lastFrame;
    fpsDeltaSum += deltaTime;
    deltaSum    += deltaTime;
    lastFrame    = currentFrame;
    frameCount++;

    if (fpsDeltaSum > FPS_UPDATE_RATE) {
        fps = frameCount / fpsDeltaSum;
        frameCount  = 0;
        fpsDeltaSum = 0;
    }

    int windowWidth;
    int windowHeight;

    // ! test with this inverted
    // if (deltaSum > *(propertiesManager->getTickRate())) {
    //     deltaSum = 0;

        windowManager->getWindowDimensions(&windowWidth, &windowHeight);

        swarm.update(
            componentsManager->getMoveComponent(),
            componentsManager->getTransformComponent(),
            *(propertiesManager->getRadiusRepulsion()),
            *(propertiesManager->getRadiusOrientation()),
            *(propertiesManager->getRadiusAttraction()),
            *(propertiesManager->getBlindAngle()),
            *(propertiesManager->getMaxForce())
        );

        move.update(
            componentsManager->getMoveComponent(),
            componentsManager->getTransformComponent(),
            *(propertiesManager->getSpeed()),
            deltaTime
        );
    // }

    render.update(windowWidth, windowHeight, componentsManager->getDrawComponent(), componentsManager->getTransformComponent());
    ui.update(propertiesManager, fps);

    windowManager->swapBuffers();
    windowManager->pollEvents();

    while (glfwGetTime() < lastFrame + *(propertiesManager->getFramerateCap())) {
        // wait
    }
}

// TODO allow keeping previous physics state copy for interpolation
// TODO implement gaffer on games game loop
// TODO test getting read only version of things I don't need to modify (copy instead of pointer to)
// TODO input manager

}
