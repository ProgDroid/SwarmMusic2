#ifndef DRAW_H
#define DRAW_H

#include <Consts.h>

#include <glm/vec4.hpp>

#include <cstdint>
#include <vector>

namespace Components {

struct Draw
{
    std::vector<glm::vec4> colour;
    std::vector<std::uint32_t> meshDataIndex;
    // ! glm::vec3 oldColour;
};

}

#endif
