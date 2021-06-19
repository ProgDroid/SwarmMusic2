#ifndef UTILS_H
#define UTILS_H

#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json.hpp>

namespace Utils
{
    void seedRNG();

    unsigned int generateRandomUInt(const unsigned int range);

    nlohmann::json loadJson(const std::string path);

    glm::quat rotationBetweenVectors(const glm::vec3 start, const glm::vec3 dest);
}

#endif
