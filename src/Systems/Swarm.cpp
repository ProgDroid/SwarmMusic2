#include <Systems/Swarm.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

namespace Systems {

Swarm::Swarm() {}

Swarm::~Swarm() {}

void Swarm::update(
    Components::Move* cMove,
    Components::Transform* cTransform,
    const int radiusRepulsion,
    const int radiusOrientation,
    const int radiusAttraction,
    const int blindAngle,
    const float maxForce
) {
    size_t toProcess = cMove->velocity.size();

    const std::uint8_t validAngle = 180 - blindAngle;

    for (size_t i = 0; i < toProcess; ++i) {
        glm::vec4 position = cTransform->position[i];
        glm::vec4 velocity = cMove->velocity[i];

        glm::vec4 repulsionVec   = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 orientationVec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 attractionVec  = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        unsigned int repulsionAgents   = 0;
        unsigned int orientationAgents = 0;
        unsigned int attractionAgents  = 0;

        for (size_t j = 0; j < toProcess; ++j) {
            if (i != j) {
                glm::vec4 vecBetweenPoints(cTransform->position[j] - position);
                float distance = glm::length(vecBetweenPoints);

                float angle    = glm::degrees(glm::angle(glm::normalize(velocity), glm::normalize(vecBetweenPoints)));

                bool isRepulsion    = angle <= validAngle && distance <= radiusRepulsion;
                repulsionVec       += vecBetweenPoints * -1.0f / distance * (float) isRepulsion;
                repulsionAgents    += isRepulsion;

                bool isOrientation  = angle <= validAngle && distance <= radiusOrientation && distance > radiusRepulsion;
                orientationVec     += cMove->velocity[j] * (float) isOrientation;
                orientationAgents  += isOrientation;

                bool isAttraction   = angle <= validAngle && distance <= radiusAttraction && distance > radiusOrientation;
                attractionVec      += vecBetweenPoints * (float) isAttraction;
                attractionAgents   += isAttraction;
            }
        }

        glm::vec4 bounds  = glm::vec4(100.0f * (position.x <= (float) CUBE_SIZE_HALF * -1.0f), 100.0f * (position.y <= (float) CUBE_SIZE_HALF * -1.0f), 100.0f * (position.z <= (float) CUBE_SIZE_HALF * -1.0f), 0.0f);
                  bounds += glm::vec4(-100.0f * (position.x >= (float) CUBE_SIZE_HALF), -100.0f * (position.y >= (float) CUBE_SIZE_HALF), -100.0f * (position.z >= (float) CUBE_SIZE_HALF), 0.0f);

        if (repulsionAgents == 0) {
            glm::vec4 orientation    = orientationVec /* * 2.5f */ / (float) std::max(orientationAgents, 1u);
            // orientation = orientation / std::max(glm::length(orientation), 1.0f) * std::min(maxForce, glm::length(orientation));

            glm::vec4 attraction     = attractionVec / (float) std::max(attractionAgents, 1u);
            // attraction = attraction / std::max(glm::length(attraction), 1.0f) * std::min(maxForce, glm::length(attraction));

            glm::vec4 acceleration   = orientation + attraction / (float) std::max(((orientationAgents > 0) + (attractionAgents > 0)), 1);

            acceleration = (acceleration / std::max(glm::length(acceleration), 1.0f)) * std::min(maxForce, glm::length(acceleration));

            cMove->velocity[i] += acceleration + bounds;
            cMove->velocity[i]  = glm::normalize(cMove->velocity[i]);
            continue;
        }

        glm::vec4 repulsion = repulsionVec / (float) repulsionAgents;
        repulsion = repulsion / std::max(glm::length(repulsion), 1.0f) * std::min(maxForce, glm::length(repulsion));

        cMove->velocity[i] += repulsion + bounds;
        cMove->velocity[i]  = glm::normalize(cMove->velocity[i]);
    }
}

}
