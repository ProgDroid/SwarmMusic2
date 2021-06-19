#ifndef MOVE_SYSTEM_H
#define MOVE_SYSTEM_H

#include <Components/Move.h>
#include <Components/Transform.h>

namespace Systems {

class Move
{
public:
    Move();
    ~Move();

    void update(Components::Move* cMove, Components::Transform* cTransform, float speed, float deltaTime);
};

}

#endif
