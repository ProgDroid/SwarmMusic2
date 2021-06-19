#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <Components/Draw.h>
#include <Components/Move.h>
#include <Components/Transform.h>

#include <nlohmann/json.hpp>

namespace Managers
{

class Component
{
private:
    Components::Draw cDraw;
    Components::Move cMove;
    Components::Transform cTransform;
    Components::Transform cTransformPrevious;

    nlohmann::json componentsData;
    nlohmann::json vertexData;

public:
    Component();
    ~Component();

    void addComponents(std::uint8_t entityType);
    void removeComponents(std::uint32_t entityIndex, std::uint8_t entityType);

    Components::Draw* getDrawComponent();
    Components::Move* getMoveComponent();
    Components::Transform* getTransformComponent();

private:
    void initDrawComponent(std::uint8_t entityType);
    void initMoveComponent();
    void initTransformComponent(std::uint8_t entityType);

    void removeDrawComponent(std::uint32_t entityIndex);
    void removeMoveComponent();
    void removeTransformComponent(std::uint32_t entityIndex);

    void reserveVectors();
};

}

#endif
