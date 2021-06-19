#include <Managers/Entity.h>

#include <Consts.h>
#include <Managers/Component.h>
#include <Utils.h>

#include <algorithm>
#include <iostream>

namespace Managers {

Entity::Entity()
    : entityList(std::vector<std::uint32_t>(MAX_ENTITIES)),
      availableIds(initQueue()),
      agents(0),
      attractors(0)
{
    entityIndices.fill(UINT32_MAX);
}

Entity::~Entity() {}

std::uint32_t Entity::spawn(std::uint8_t type) {
    std::uint8_t canAddAgent     = agents < MAX_AGENTS;
    std::uint8_t canAddAttractor = attractors < MAX_ATTRACTORS;

    std::uint8_t addingAgent     = type & AGENT & canAddAgent;
    std::uint8_t addingAttractor = type >> AGENT & canAddAttractor;

    if (addingAgent || addingAttractor) {
        agents     += addingAgent;
        attractors += addingAttractor;

        std::uint32_t id = availableIds.top();
        availableIds.pop();

        entityList.push_back(id);
        entityIndices[id] = entityList.size() - 1;

        return id;
    }

    return UINT32_MAX;
}

std::uint32_t Entity::destroy(std::uint32_t entityId, std::uint8_t type) {
    std::uint8_t canDestroyAgent     = agents > 0;
    std::uint8_t canDestroyAttractor = attractors > 0;

    std::uint8_t destroyingAgent     = type & AGENT & canDestroyAgent;
    std::uint8_t destroyingAttractor = type >> AGENT & canDestroyAttractor;

    std::uint32_t index = entityIndices[entityId];

    if ((destroyingAgent || destroyingAttractor) && index < MAX_ENTITIES) {
        agents     -= destroyingAgent;
        attractors -= destroyingAttractor;

        entityIndices[entityId] = UINT32_MAX;
        entityIndices[entityList[entityList.size() - 1]] = index;

        std::swap(entityList[index], entityList[entityList.size() - 1]);
        entityList.pop_back();

        availableIds.push(entityId);

        return index;
    }

    return UINT32_MAX;
}

std::stack<std::uint32_t, std::vector<std::uint32_t> > Entity::initQueue() {
    std::vector<std::uint32_t> stackVector(MAX_ENTITIES);
    std::stack<std::uint32_t, std::vector<std::uint32_t> > stack(stackVector);

    for (std::size_t i = MAX_ENTITIES; i > 0; --i) {
        stack.push(i - 1);
    }

    return stack;
}

}