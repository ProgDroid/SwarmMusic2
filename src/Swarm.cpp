/**
 * A swarm object, made up of agents and attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glm/glm.hpp>
#include <Shader.h>

#include <random>
#include <vector>

#include <Agent.h>
#include <Attractor.h>
#include <Swarm.h>
#include <Triplet.h>

const int FREEFORM       = 0;
const int AVERAGE        = 1;

const int MAX_ATTRACTORS = 14;
const int MAX_SIZE       = 500;

const int RANDOM         = 0;
const int SCALES         = 1;

/**
 * Random number generator
 *
 * @return float
 */
float swarmRand() {
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 20);

    return (float)distribution(generator);
}

Swarm::Swarm() {
    agents.reserve(MAX_SIZE);
    attractors.reserve(MAX_ATTRACTORS);
    addAgents();

    radiusRepulsion   = 20.0f;
    radiusOrientation = 50.0f;
    radiusAttraction  = 110.0f;
    blindAngle        = 10.0f;
    speed             = 30.5f;
    maxForce          = 30.7f;
    swarmMode         = RANDOM;

    Triplet averagePosition(0.0, 0.0, 0.0);
}

int Swarm::getSize() {
    return this->agents.size();
}

int Swarm::getAttractorsCount() {
    return this->attractors.size();
}

float Swarm::getRepulsionRadius() {
    return this->radiusRepulsion;
}

float Swarm::getOrientationRadius() {
    return this->radiusOrientation;
}

float Swarm::getAttractionRadius() {
    return this->radiusAttraction;
}

void Swarm::setRepulsionRadius(float value) {
    this->radiusRepulsion = value;
}

void Swarm::setOrientationRadius(float value) {
    this->radiusOrientation = value;
}

void Swarm::setAttractionRadius(float value) {
    this->radiusAttraction = value;
}

float Swarm::getBlindAngle() {
    return this->blindAngle;
}

float Swarm::getSpeed() {
    return this->speed;
}

float Swarm::getMaxForce() {
    return this->maxForce;
}

void Swarm::setBlindAngle(float value) {
    this->blindAngle = value;
}

void Swarm::setSpeed(float value) {
    this->speed = value;
}

void Swarm::setMaxForce(float value) {
    this->maxForce = value;
}

int Swarm::getSwarmMode() {
    return this->swarmMode;
}

void Swarm::setSwarmMode(int value) {
    this->swarmMode = value;
}

Triplet Swarm::getAveragePosition() {
    return this->averagePosition;
}

void Swarm::addAgents() {
    for (int i = 0; i < MAX_SIZE; ++i) {
        Agent agent;
        agents.push_back(agent);
    }
}

void Swarm::resetAll() {
    agents.erase(agents.begin(), agents.end());

    for (int i = 0; i < MAX_SIZE; ++i) {
        Agent agent;
        agents.push_back(agent);
    }
}

void Swarm::resetAttractors() {
    attractors.erase(attractors.begin(), attractors.end());
}

void Swarm::addAttractor(int pitch, unsigned int *VBO, unsigned int *EBO, unsigned int *VAO, int tone = -1) {
    Attractor attractor(pitch, tone);
    attractors.push_back(attractor);
    setupDrawAttractors(VBO, EBO, VAO);
}

void Swarm::swarm(float deltaTime) {
    for (int i = 0, size = getSize(); i < size; ++i)     {
        agents[i].step(agents, radiusRepulsion, radiusOrientation, radiusAttraction, blindAngle, maxForce);

        float positionX = swarmMode == AVERAGE ? averagePosition.getX() + agents[i].getPosition().getX() : agents[i].getPosition().getX();
        float positionY = swarmMode == AVERAGE ? averagePosition.getY() + agents[i].getPosition().getY() : agents[i].getPosition().getY();
        float positionZ = swarmMode == AVERAGE ? averagePosition.getZ() + agents[i].getPosition().getZ() : agents[i].getPosition().getZ();

        if (swarmMode == RANDOM) {
            if (swarmRand() < 2) {
                averagePosition.setX(positionX);
            }
            if (swarmRand() < 2) {
                averagePosition.setY(positionY);
            }
            if (swarmRand() < 2) {
                averagePosition.setZ(positionZ);
            }

            continue;
        }

        averagePosition.setX(positionX);
        averagePosition.setY(positionY);
        averagePosition.setZ(positionZ);
    }

    for (int i = 0, size = getSize(); i < size; ++i) {
        agents[i].move(speed, attractors, deltaTime);
    }

    if (swarmMode == AVERAGE) {
        averagePosition.scalarDiv(getSize());
    }
}

void Swarm::setupDrawAgents(unsigned int *VBO, unsigned int *normalVBO, unsigned int *EBO, unsigned int *VAO) {
    agents.at(0).setupDraw(VBO, normalVBO, EBO, VAO);
}

void Swarm::setupDrawAttractors(unsigned int *VBO, unsigned int *EBO, unsigned int *VAO) {
    attractors.at(0).setupDraw(VBO, EBO, VAO);
}

void Swarm::drawAgents(Shader shader) {
    for (int i = 0, size = getSize(); i < size; ++i) {
        glm::mat4 agentModel = glm::mat4(1.0f);
        agents.at(i).transform(&agentModel);

        Triplet agentColour    = agents.at(i).getColour();
        Triplet agentOldColour = agents.at(i).getOldColour();

        glm::vec3 agentColourVec    = glm::vec3(agentColour.getX(), agentColour.getY(), agentColour.getZ());
        glm::vec3 agentOldColourVec = glm::vec3(agentOldColour.getX(), agentOldColour.getY(), agentOldColour.getZ());

        glm::vec3 objColour = agentColourVec;

        if (agentColourVec.x != agentOldColourVec.x) {
            int swapTime = agents.at(i).getColourSwapTime();
            objColour = glm::mix(agentOldColourVec, agentColourVec, (swapTime / 100.0f));
            if (swapTime == 100) {
                swapTime = -1;
                agents.at(i).setOldColour(agentColour);
            }
            agents.at(i).setColourSwapTime(swapTime + 1);
        }

        shader.setMat4("model", agentModel);
        shader.setVec3("material.ambient", objColour);
        shader.setVec3("material.diffuse", objColour);
        shader.setVec3("material.specular", glm::vec3(0.25f, 0.25f, 0.25f));
        shader.setFloat("material.shininess", 32.0f);

        agents.at(i).draw();
    }
}

void Swarm::drawAttractors(Shader shader) {
    if (getAttractorsCount() == 0) {
        return;
    }

    for (int i = 0, size = getAttractorsCount(); i < size; ++i) {
        glm::mat4 attractorModel = glm::mat4(1.0f);
        attractors.at(i).transform(&attractorModel);

        shader.setMat4("model", attractorModel);
        shader.setVec3("material.ambient", glm::vec3(attractors.at(i).getColour().getX(), attractors.at(i).getColour().getY(), attractors.at(i).getColour().getZ()));
        shader.setVec3("material.diffuse", glm::vec3(attractors.at(i).getColour().getX(), attractors.at(i).getColour().getY(), attractors.at(i).getColour().getZ()));
        shader.setVec3("material.specular", glm::vec3(0.25f, 0.25f, 0.25f));
        shader.setFloat("material.shininess", 32.0f);

        attractors.at(i).draw();
    }
}
