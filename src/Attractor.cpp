/**
 * An attracting particle in the swarm to conduct agents
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <random>
#include <vector>

#include <Attractor.h>
#include <Triplet.h>

const float CUBE_HALF_SIZE = 400.0;
const float MAUVE_R        = 0.75686274509;
const float MAUVE_G        = 0.51372549019;
const float MAUVE_B        = 0.62352941176;
const float FREEFORM       = 0;
const float SCALES         = 1;
const float I              = 0;
const float ii             = 1;
const float iii            = 2;
const float IV             = 3;
const float V              = 4;
const float vi             = 5;
const float vii            = 6;
const float PI             = 3.14159265;

/**
 * Random number generator
 *
 * @return float
 */
float attractRand() {
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 800);

    return (float) distribution(generator);
}

Attractor::Attractor(int pitch, int givenTone) {
    tone = givenTone;
    mode = SCALES;

    float xCoord  = (float) pitch - 72;
          xCoord *= 800;
          xCoord /= 24;
          xCoord -= 400;

    position = Triplet(xCoord, attractRand() - CUBE_HALF_SIZE, attractRand() - CUBE_HALF_SIZE);

    if (tone == I || tone == V) {
        strength = 3;
    } else if (tone == ii || tone == IV || tone == vii) {
        strength = 2;
    } else if (tone == iii || tone == vi || tone == -1) {
        strength = 1;
    }

    float modifier = strength == 1 ? -0.2 : (strength == 2 ? 0 : 0.2);

    colour = Triplet(MAUVE_R + modifier, MAUVE_G + modifier, MAUVE_B + modifier);
}

Triplet Attractor::getPosition() {
    return this->position;
}

/**
 * Setup sphere drawing
 *
 * @param unsigned int *VBO
 * @param unsigned int *EBO
 * @param unsigned int *VAO
 * @return void
 */
void Attractor::setupDraw(unsigned int *VBO, unsigned int *EBO, unsigned int *VAO) {
    std::vector<float> vertices;

    for (int i = 0; i <= 25; ++i) {
        float stackAngle = (PI / 2) - (i * (PI / 25));
        float xy         = cos(stackAngle);
        float z          = sin(stackAngle);

        for (int j = 0; j <= 25; ++j) {
            float sectorAngle = j * ((2 * PI) / 25);

            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(colour.getX());
            vertices.push_back(colour.getY());
            vertices.push_back(colour.getZ());
        }
    }

    std::vector<int> indices;

    for (int i = 0; i < 25; ++i) {
        int k1 = i * 26;
        int k2 = k1 + 26;

        for (int j = 0; j < 25; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != 24) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glGenVertexArrays(1, VAO);

    glBindVertexArray(*VAO);

    // vertices and colour
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

/**
 * Transform attractor model
 *
 * @param glm::mat4 *attractorModel
 * @return void
 */
void Attractor::transform(glm::mat4 *attractorModel) {
    *attractorModel = glm::translate(*attractorModel, glm::vec3(position.getX(), position.getY(), position.getZ()));
    *attractorModel = glm::scale(*attractorModel, glm::vec3(5.0f, 5.0f, 5.0f));
}

/**
 * Draw attractor
 *
 * @return void
 */
void Attractor::draw() {
    glDrawElements(GL_TRIANGLES, 3600, GL_UNSIGNED_INT, 0);
}
