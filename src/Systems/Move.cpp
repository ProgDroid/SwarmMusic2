#include <Systems/Move.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

namespace Systems {

Move::Move() {}

Move::~Move() {}

void Move::update(Components::Move* cMove, Components::Transform* cTransform, float speed, float deltaTime) {
    size_t toMove = cMove->velocity.size();

    for (size_t i = 0; i < toMove; ++i) {
        cTransform->rotation[i]  = cMove->velocity[i];
        cTransform->position[i] += cMove->velocity[i] * speed /* * deltaTime * 10.0f */;
    }
}

}
