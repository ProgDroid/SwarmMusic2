#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <Components/Draw.h>
#include <Components/Transform.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <Shader.h>

#include <cstdint>

namespace Systems {

class Render
{
private:
    std::uint32_t colourBuffers[3];
    std::uint32_t EBOs[3];
    std::uint32_t modelBuffers[3];
    std::uint32_t VAOs[3];
    std::uint32_t VBOs[6];

    nlohmann::json meshData;

    Shader solid;

public:
    Render();
    ~Render();

    void update(int width, int height, Components::Draw* cDraw, Components::Transform* cTransform);

private:
    void transform(
        const size_t toRender,
        Components::Transform* cTransform,
        Components::Draw* cDraw,
        std::array<std::vector<glm::mat4>, 3>* modelMatrices,
        std::array<std::vector<glm::vec4>, 3>* colourMatrices,
        std::array<size_t, 3>* sizes
    );

    Shader loadShader(const std::string shaderName) const;
};

}

#endif
