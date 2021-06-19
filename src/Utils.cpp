#include <Utils.h>

#include <Consts.h>

#include <fstream>
#include <random>

#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

std::random_device rd;
std::mt19937       rng;

namespace Utils
{

void seedRNG() {
    rng.seed(rd());
}

unsigned int generateRandomUInt(const unsigned int range) {
    std::uniform_int_distribution<> distribution(0, range);

    return distribution(rng);
}

nlohmann::json loadJson(std::string path) {
    nlohmann::json json;

    std::ifstream inputData(path);
    if (!inputData) {
        throw std::runtime_error("Data file not found at: " + path);
    }

    inputData >> json;

    return json;
}

glm::quat rotationBetweenVectors(const glm::vec3 start, const glm::vec3 dest) {
    glm::vec3 rotationAxis;

    float cosTheta = glm::dot(start, dest);

    if (cosTheta < -1 + 0.001f) {
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);

        if (glm::length2(rotationAxis) < 0.01) {
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
        }

        rotationAxis = glm::normalize(rotationAxis);

        return glm::angleAxis(glm::radians(180.0f), rotationAxis);
    }

    rotationAxis = cross(start, dest);

    float s   = sqrt((1 + cosTheta) * 2);
    float inv = 1 / s;

    return glm::quat(s * 0.5f, rotationAxis.x * inv, rotationAxis.y * inv, rotationAxis.z * inv);
}

}
