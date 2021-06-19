#ifndef MOVE_H
#define MOVE_H

#include <Consts.h>

#include <glm/vec4.hpp>

#include <vector>

namespace Components {

struct Move
{
    std::vector<glm::vec4> velocity;
};

}

#endif
