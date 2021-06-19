#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <Consts.h>

#include <glm/vec4.hpp>

#include <vector>

namespace Components {

struct Transform
{
    std::vector<glm::vec4> position;
    std::vector<glm::vec4> rotation;
};

}

#endif
