/**
 * An agent of the swarm, with its local rules for movement
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include <Agent.h>
#include <Attractor.h>
#include <Triplet.h>

const float TO_DEGREES   = 57.295779524;
const float MIN_DISTANCE = 100000000;
const float BOUNDARY     = 1000.0;
const float PI           = 3.14159265;

/**
 * Random number generator
 *
 * Generates numbers from 0 to CUBE_HALF_SIZE * 2 which can then be used for the position coordinates
 * directly by adjusting to -CUBE_HALF_SIZE to CUBE_HALF_SIZE
 * Also used for the direction, adjusting similarly and then normalising
 *
 * @return float
 */
float agentRand() {
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, CUBE_HALF_SIZE * 2);

    return (float) distribution(generator);
}

Agent::Agent() {
    // assign random colour
    float randomValue = agentRand();

    colour = randomValue >= 0 && randomValue <= (CUBE_HALF_SIZE * 2) ? (randomValue <= CUBE_HALF_SIZE ? BLUE : RED) : WHITE;

    oldColour = colour;

    position  = Triplet(agentRand() - CUBE_HALF_SIZE, agentRand() - CUBE_HALF_SIZE, agentRand() - CUBE_HALF_SIZE);
    direction = Triplet((agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE, (agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE, (agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE);
}

Triplet Agent::getPosition() {
    return this->position;
}

Triplet Agent::getColour() {
    return this->colour;
}

Triplet Agent::getOldColour() {
    return this->oldColour;
}

void Agent::setOldColour(Triplet value) {
    oldColour = value;
}

int Agent::getColourSwapTime() {
    return this->colourSwapTime;
}

void Agent::setColourSwapTime(int value) {
    colourSwapTime = value;
}

/**
 * Change colour of an agent if orientation area neighbours are mostly of the other colour
 *
 * @param int colourCount Positive (+) value for blue, negative (-) for red
 * @return void
 */
void Agent::setColour(int colourCount) {
    if (colourCount == 0) {
        return;
    }

    if (/* colour.getX() != oldColour.getX() &&  */colourSwapTime == 0) {
        oldColour = colour;
        colour    = colourCount > 0 ? BLUE : RED;
    }
}

void Agent::computeChange(Triplet newDirection, float count, Triplet direction, float maxForce) {
    newDirection.scalarDiv(count);
    newDirection.normalise();
    newDirection = newDirection - direction;
    // truncate to maximum force
    if (newDirection.length() > maxForce) {
        newDirection.normalise();
        newDirection.scalarMul(maxForce);
    }
}

/**
 * Compute agent movement when in repulsion region
 *
 * @param std::vector<Agent> &agents
 * @param float radiusRepulsion
 * @param float blindAngle
 * @param float maxForce
 */
Triplet Agent::repulsion(
    std::vector<Agent> &agents,
    float radiusRepulsion,
    float blindAngle,
    float maxForce
) {
    Triplet newDirection(0.0, 0.0, 0.0);
    int count = 0;

    for (std::size_t i = 0, size = agents.size(); i < size; ++i) {
        // calculate distance to another agent
        float distance = position.distance(agents.at(i).position);
        // if not itself and within radius
        if (distance > 0 && distance <= radiusRepulsion) {
            // check if not behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle)) {
                // invert vector
                vectorBetweenPoints.scalarMul(-1);
                vectorBetweenPoints.normalise();
                vectorBetweenPoints.scalarDiv(distance);
                newDirection = newDirection + vectorBetweenPoints;
                ++count;
            }
        }
    }

    // average and add to acceleration
    if (count > 0) {
        computeChange(newDirection, (float) count, direction, maxForce);
    }

    return newDirection;
}

/**
 * Compute agent movement when in orientation region
 *
 * @param std::vector<Agent> &agents
 * @param float radiusRepulsion
 * @param float radiusOrientation
 * @param float blindAngle
 * @param float maxForce
 */
Triplet Agent::orientation(
    std::vector<Agent> &agents,
    float radiusRepulsion,
    float radiusOrientation,
    float blindAngle,
    float maxForce
) {
    Triplet newDirection(0.0, 0.0, 0.0);
    int count       = 0;
    int colourCount = 0;

    for (std::size_t i = 0, size = agents.size(); i < size; ++i) {
        // calculate distance
        float distance = position.distance(agents.at(i).position);
        // if within range (also not itself)
        if (distance > radiusRepulsion && distance <= radiusOrientation) {
            // check if behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle)) {
                newDirection = newDirection + agents.at(i).direction;
                ++count;

                // check colour R value
                // if 1.0 it is red so take 1, else add 1 (is blue)
                if (agents.at(i).getColour().getX() == 1.0) {
                    --colourCount;
                } else {
                    ++colourCount;
                }
            }
        }
    }

    if (count > 0) {
        // average and add to acceleration
        computeChange(newDirection, (float) count, direction, maxForce);
    }

    setColour(colourCount);

    return newDirection;
}

/**
 * Compute agent movement when in attraction region
 *
 * @param std::vector<Agent> &agents
 * @param float radiusOrientation
 * @param float radiusAttraction
 * @param float blindAngle
 * @param float maxForce
 */
Triplet Agent::attraction(
    std::vector<Agent> &agents,
    float radiusOrientation,
    float radiusAttraction,
    float blindAngle,
    float maxForce
) {
    Triplet newDirection(0.0, 0.0, 0.0);
    int count = 0;

    for (std::size_t i = 0, size = agents.size(); i < size; ++i) {
        // calculate distance
        float distance = position.distance(agents.at(i).position);
        // if within range
        if (distance > radiusOrientation && distance <= radiusAttraction) {
            // check if behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle)) {
                newDirection = newDirection + vectorBetweenPoints;
                ++count;
            }
        }
    }

    if (count > 0) {
        // average and add to acceleration
        computeChange(newDirection, (float) count, direction, maxForce);
    }

    return newDirection;
}

/**
 * Rule to enforce cube boundaries on agent
 *
 * Explicit ifs for clear purposes
 *
 * @param Triplet min
 * @param Triplet max
 * @return Triplet
 */
Triplet Agent::bounding() {
    Triplet vector(0.0, 0.0, 0.0);

    if (position.getX() <= MIN.getX()) {
        vector.setX(BOUNDARY);
    } else if (position.getX() >= MAX.getX()) {
        vector.setX(-BOUNDARY);
    }

    if (position.getY() <= MIN.getY()) {
        vector.setY(BOUNDARY);
    } else if (position.getY() >= MAX.getY()) {
        vector.setY(-BOUNDARY);
    }

    if (position.getZ() <= MIN.getZ()) {
        vector.setZ(BOUNDARY);
    } else if (position.getZ() >= MAX.getZ()) {
        vector.setZ(-BOUNDARY);
    }

    return vector;
}

/**
 * Setup an agent to be drawn
 *
 * @param unsigned int *VBO
 * @param unsigned int *normalVBO
 * @param unsigned int *EBO
 * @param unsigned int *VAO
 *
 * @return void
 */
void Agent::setupDraw(unsigned int *VBO, unsigned int *normalVBO, unsigned int *EBO, unsigned int *VAO) {
    // cone vertices
    int points = 10;

    float angle     = 0.0f;
    float increment = glm::radians(360.0f) / (float) points;

    std::vector<float> vertices;

    // bottom circle vertices
    for (int i = 0; i < points; ++i) {
        vertices.push_back(cos(angle) * 0.1);
        vertices.push_back(-0.2f);
        vertices.push_back(sin(angle) * 0.1);
        angle += increment;
    }

    // top vertex
    vertices.push_back(0.0f);
    vertices.push_back(0.3f);
    vertices.push_back(0.0f);

    // bottom vertex
    vertices.push_back(0.0f);
    vertices.push_back(-0.2f);
    vertices.push_back(0.0f);

    unsigned int indices[] = {
        10,  0, 1,
        10,  1, 2,
        10,  2, 3,
        10,  3, 4,
        10,  4, 5,
        10,  5, 6,
        10,  6, 7,
        10,  7, 8,
        10,  8, 9,
        10,  9, 0,
        11,  0, 1,
        11,  1, 2,
        11,  2, 3,
        11,  3, 4,
        11,  4, 5,
        11,  5, 6,
        11,  6, 7,
        11,  7, 8,
        11,  8, 9,
        11,  9, 0
    };

    std::vector<float> normals;

    for (int i = 0; i < points; ++i) {
        int index = i * 3;
        glm::vec3 a(vertices.at(index), vertices.at(index + 1), vertices.at(index + 2));

        index = (i + 1) * 3;
        if (i == 9) {
            index = 0;
        }
        glm::vec3 b(vertices.at(index), vertices.at(index + 1), vertices.at(index + 2));
        glm::vec3 crossProduct = glm::cross(glm::normalize(b), glm::normalize(a));
        normals.push_back(crossProduct.x);
        normals.push_back(crossProduct.y);
        normals.push_back(crossProduct.z);
    }

    for (int i = 0; i < points; ++i) {
        normals.push_back(0.0f);
        normals.push_back(-1.0f);
        normals.push_back(0.0f);
    }

    glGenBuffers(1, VBO);
    glGenBuffers(1, normalVBO);
    glGenBuffers(1, EBO);
    glGenVertexArrays(1, VAO);

    glBindVertexArray(*VAO);

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // normals
    glBindBuffer(GL_ARRAY_BUFFER, *normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
}

glm::quat Agent::rotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
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

/**
 * Transform agent model
 *
 * @param glm::mat4 *agentModel
 * @return void
 */
void Agent::transform(glm::mat4 *agentModel) {
    glm::vec3 start = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 dest  = glm::vec3(direction.getX(), direction.getY(), direction.getZ());

    glm::quat quaternion        = rotationBetweenVectors(start, dest);
    glm::mat4 scalingMatrix     = glm::scale(*agentModel, glm::vec3(20.0f, 20.0f, 20.0f));
    glm::mat4 rotationMatrix    = glm::toMat4(quaternion);
    glm::mat4 translationMatrix = glm::translate(*agentModel, glm::vec3(position.getX(), position.getY(), position.getZ()));

    *agentModel = translationMatrix * rotationMatrix * scalingMatrix;
}

/**
 * Draw an agent
 *
 * @return void
 */
void Agent::draw() {
    glDrawElements(GL_TRIANGLE_FAN, 30, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_TRIANGLE_FAN, 30, GL_UNSIGNED_INT, (void*) (30 * sizeof(float)));
}

/**
 * Move agent
 *
 * @param float speed
 * @param std::vector<Attractor> attractors
 * @return void
 */
void Agent::move(float speed, std::vector<Attractor> attractors, float deltaTime) {
    if (attractors.size() != 0) {
        float  minDistance = MIN_DISTANCE;
        size_t index       = 0;

        for (std::size_t i = 0, size = attractors.size(); i < size; ++i) {
            float distance = position.distance(attractors.at(i).getPosition());
            if (distance < minDistance) {
                index       = i;
                minDistance = distance;
            }
        }

        Triplet newTmp = attractors.at(index).getPosition() - position;
        newTmp.scalarMul(0.0005);
        acceleration = acceleration + newTmp;
    }

    direction = direction + acceleration;
    direction.normalise();

    Triplet stepDirection = Triplet(direction.getX(), direction.getY(), direction.getZ());
    stepDirection.scalarMul(speed);
    stepDirection.scalarMul(deltaTime);
    position = position + stepDirection;

    // reset acceleration
    acceleration.scalarMul(0);
}

/**
 * Compute a step for this agent
 *
 * @param std::vector<Agent> &agents
 * @param float radiusRepulsion
 * @param float radiusOrientation
 * @param float radiusAttraction
 * @param float angle
 * @param float maxForce
 */
void Agent::step(
    std::vector<Agent> &agents,
    float radiusRepulsion,
    float radiusOrientation,
    float radiusAttraction,
    float angle,
    float maxForce
) {
    // calculate repulsion first as this is the priority
    Triplet repulsionVector = repulsion(agents, radiusRepulsion, angle, maxForce);

    // if there's moving away to do, do it and early return
    if (repulsionVector.length() != 0) {
        acceleration = acceleration + repulsionVector;
        return;
    }

    // otherwise we can seek others, stay in course or both
    Triplet orientationVector = orientation(agents, radiusRepulsion, radiusOrientation, angle, maxForce);
    orientationVector.scalarMul(2.5); // For smoother movement

    Triplet attractionVector  = attraction(agents, radiusOrientation, radiusAttraction, angle, maxForce);

    Triplet tmpVector = orientationVector + attractionVector;
    if (orientationVector.length() != 0 && attractionVector.length() != 0) {
        tmpVector.scalarDiv(2);
    }

    Triplet boundingVector = bounding();

    acceleration = acceleration + tmpVector + boundingVector;
}
