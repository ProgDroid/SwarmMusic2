#ifndef SWARM_SYSTEM_H
#define SWARM_SYSTEM_H

#include <Components/Move.h>
#include <Components/Transform.h>

#include <glm/vec4.hpp>

namespace Systems {

class Swarm
{
public:
    Swarm();
    ~Swarm();

    void update(
        Components::Move* cMove,
        Components::Transform* cTransform,
        const int radiusRepulsion,
        const int radiusOrientation,
        const int radiusAttraction,
        const int blindAngle,
        const float maxForce
    );

private:
    glm::vec4 attraction();
    glm::vec4 orientation();
    glm::vec4 repulsion();
    glm::vec4 bindToCube();
};

}

#endif
