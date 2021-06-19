#include <Managers/Component.h>

#include <Consts.h>
#include <Utils.h>

#include <iostream>

namespace Managers
{

Component::Component()
    : componentsData(Utils::loadJson(COMPONENTS_DATA_PATH))
{
    reserveVectors();
}

Component::~Component() {}

void Component::addComponents(std::uint8_t entityType) {
    initDrawComponent(entityType);

    initTransformComponent(entityType);

    if (entityType & AGENT) {
        initMoveComponent();
    }
}

void Component::removeComponents(std::uint32_t entityIndex, std::uint8_t entityType) {
    removeDrawComponent(entityIndex);
    removeTransformComponent(entityIndex);

    if (entityType & AGENT) {
        removeMoveComponent();
    }
}

Components::Draw* Component::getDrawComponent() {
    return &cDraw;
}

Components::Move* Component::getMoveComponent() {
    return &cMove;
}

Components::Transform* Component::getTransformComponent() {
    return &cTransform;
}

void Component::initDrawComponent(std::uint8_t entityType) {
    unsigned int colour = Utils::generateRandomUInt(entityType);

    cDraw.colour.push_back(
        glm::vec4(
            (float) componentsData["draw"][entityType - 1]["colours"][colour][0],
            (float) componentsData["draw"][entityType - 1]["colours"][colour][1],
            (float) componentsData["draw"][entityType - 1]["colours"][colour][2],
            (float) componentsData["draw"][entityType - 1]["colours"][colour][3]
        )
    );

    cDraw.meshDataIndex.push_back(entityType - 1);
}

void Component::initMoveComponent() {
    cMove.velocity.push_back(
        glm::vec4(
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF) / CUBE_SIZE_HALF,
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF) / CUBE_SIZE_HALF,
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF) / CUBE_SIZE_HALF,
            0.f
        )
    );
}

void Component::initTransformComponent(std::uint8_t entityType) {
    cTransform.position.push_back(
        glm::vec4(
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF),
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF),
            ((float) Utils::generateRandomUInt(CUBE_SIZE) - CUBE_SIZE_HALF),
            0.f
        )
    );

    cTransform.rotation.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void Component::removeDrawComponent(std::uint32_t entityIndex) {
    // ! assumption that all should be the same size
    size_t end = cDraw.colour.size() - 1;

    std::swap(cDraw.colour[entityIndex], cDraw.colour[end]);
    cDraw.colour.pop_back();

    std::swap(cDraw.meshDataIndex[entityIndex], cDraw.meshDataIndex[end]);
    cDraw.meshDataIndex.pop_back();
}

void Component::removeMoveComponent() {
    cMove.velocity.pop_back();
}

void Component::removeTransformComponent(std::uint32_t entityIndex) {
    // ! assumption that all should be the same size
    size_t end = cTransform.position.size() - 1;

    std::swap(cTransform.position[entityIndex], cTransform.position[end]);
    cTransform.position.pop_back();

    std::swap(cTransform.rotation[entityIndex], cTransform.rotation[end]);
    cTransform.rotation.pop_back();
}

void Component::reserveVectors() {
    cDraw.colour.reserve(MAX_ENTITIES);
    cDraw.meshDataIndex.reserve(MAX_ENTITIES);

    cMove.velocity.reserve(MAX_ENTITIES);

    cTransform.position.reserve(MAX_ENTITIES);
    cTransform.rotation.reserve(MAX_ENTITIES);
}

}