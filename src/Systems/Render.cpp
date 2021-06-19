#include <Systems/Render.h>

#include <Consts.h>
#include <Utils.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <array>
#include <vector>

namespace Systems {

Render::Render()
    : meshData(Utils::loadJson(MESH_DATA_PATH)),
      solid(loadShader("light"))
{
    glPrimitiveRestartIndex(UINT16_MAX);

    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(6, VBOs);
    glGenBuffers(3, EBOs);
    glGenVertexArrays(3, VAOs);

    glGenBuffers(3, colourBuffers);
    glGenBuffers(3, modelBuffers);

    std::array<size_t, 3> sizes;
    sizes[0] = MAX_AGENTS;
    sizes[1] = MAX_ATTRACTORS;
    sizes[2] = 1; // wire cube

    for (size_t i = 0; i < 3; ++i) {
        glBindVertexArray(VAOs[i]);

        std::vector<float> vertices = meshData["vertices"][i];
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i * 2]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        std::vector<std::uint32_t> indices = meshData["indices"][i];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(std::uint32_t), (void*) 0);
        glEnableVertexAttribArray(0);

        std::vector<float> normals = meshData["normals"][i];
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i * 2 + 1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, modelBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER, sizes[i] * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, colourBuffers[i]);
        glBufferData(GL_ARRAY_BUFFER, sizes[i] * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

Render::~Render() {
    glDeleteBuffers(3, VAOs);
    glDeleteBuffers(6, VBOs);
    glDeleteBuffers(3, EBOs);

    glDeleteBuffers(3, modelBuffers);
    glDeleteBuffers(3, colourBuffers);
}

void Render::update(int width, int height, Components::Draw* cDraw, Components::Transform* cTransform) {
    glClearColor(0.23529411764, 0.23529411764, 0.23529411764, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ! change glfwGetTime to angle theta based on keyboard input
    glm::mat4 view = glm::lookAt(glm::vec3(1500.0f * sin(glfwGetTime() * (PI / 180.0f)), 500.0f, 1500.0f * cos(glfwGetTime() * (PI / 180.0f))),
                                 glm::vec3(0.0f, -80.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 2500.0f);

    // assume all of same size
    const std::size_t toRender = cDraw->colour.size();

    std::size_t vec4Size = sizeof(glm::vec4);

    size_t allVAOs = sizeof(VAOs) / sizeof(VAOs[0]);

    std::array<std::vector<glm::mat4>, 3> modelMatrixArrays;
    std::array<std::vector<glm::vec4>, 3> colourVectorArrays;
    std::array<size_t, 3>                 sizes;

    sizes.fill(0);

    transform(toRender, cTransform, cDraw, &modelMatrixArrays, &colourVectorArrays, &sizes);

    for (size_t i = 0; i < allVAOs; ++i) {
        glBindVertexArray(VAOs[i]);

        glBindBuffer(GL_ARRAY_BUFFER, modelBuffers[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizes[i] * sizeof(glm::mat4), modelMatrixArrays[i].data());

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*) 0);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*) vec4Size);
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*) (2 * vec4Size));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*) (3 * vec4Size));
        glEnableVertexAttribArray(5);

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);

        glBindBuffer(GL_ARRAY_BUFFER, colourBuffers[i]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizes[i] * vec4Size, colourVectorArrays[i].data());

        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vec4Size, (void*) 0);
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(6, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // move these to lightData
    solid.use();
    solid.setMat4("view", view);
    solid.setMat4("projection", projection);
    solid.setVec3("lightPos", glm::vec3(0.0f, 420.0f, 0.0f));
    solid.setVec3("light.ambient",  glm::vec3(0.2f, 0.2f, 0.2f));
    solid.setVec3("light.diffuse",  glm::vec3(0.75f, 0.75f, 0.75f));
    solid.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    solid.setVec3("light.position", glm::vec3(0.0f, 420.0f, 0.0f));

    for (size_t i = 0; i < allVAOs; ++i) {
        solid.setVec3(
            "specular",
            glm::vec3(
                meshData["specular"][i][0],
                meshData["specular"][i][1],
                meshData["specular"][i][2]
            )
        );
        solid.setFloat("shininess", meshData["shininess"][i]);

        glBindVertexArray(VAOs[i]);
        glDrawElementsInstanced(meshData["mode"][i], meshData["count"][i], meshData["type"][i], (void*) 0, sizes[i]);
        glBindVertexArray(0);
    }
}

void Render::transform(
    const size_t toRender,
    Components::Transform* cTransform,
    Components::Draw* cDraw,
    std::array<std::vector<glm::mat4>, 3>* modelMatrices,
    std::array<std::vector<glm::vec4>, 3>* colourVectors,
    std::array<size_t, 3>* sizes
) {
    for (size_t i = 0; i < toRender; ++i) {
        glm::mat4 model = glm::mat4(1.0f);

        model  = glm::translate(model, glm::vec3(cTransform->position[i]));
        model *= glm::toMat4(Utils::rotationBetweenVectors(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(cTransform->rotation[i])));
        model  = glm::scale(model, glm::vec3(meshData["scaleFactor"][cDraw->meshDataIndex[i]]));

        (*modelMatrices)[cDraw->meshDataIndex[i]].push_back(model);

        (*colourVectors)[cDraw->meshDataIndex[i]].push_back(cDraw->colour[i]);

        (*sizes)[cDraw->meshDataIndex[i]]++;
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::scale(model, glm::vec3(meshData["scaleFactor"][2]));

    (*modelMatrices)[2].push_back(model);
    (*colourVectors)[2].push_back(glm::vec4(0.01f, 0.01f, 0.01f, 1.0f));
    (*sizes)[2]++;
}

Shader Render::loadShader(const std::string shaderName) const {
    std::string vs = SHADERS_PATH + shaderName + ".vs";
    std::string fs = SHADERS_PATH + shaderName + ".fs";

    return Shader(vs.c_str(), fs.c_str());
}

}
