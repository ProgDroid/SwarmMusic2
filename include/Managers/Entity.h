#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <array>
#include <cstdint>
#include <stack>
#include <vector>

namespace Managers {

class Entity
{
private:
    std::array<std::uint32_t, 5013> entityIndices; // MAX_ENTITIES

    std::vector<std::uint32_t> entityList;

    std::stack<std::uint32_t, std::vector<std::uint32_t> > availableIds;

    std::uint16_t agents;

    std::uint8_t attractors;

public:
    Entity();
    ~Entity();

    std::uint32_t spawn(std::uint8_t type);
    std::uint32_t destroy(std::uint32_t entityId, std::uint8_t type);

private:
    std::stack<std::uint32_t, std::vector<std::uint32_t> > initQueue();
};

}

#endif
