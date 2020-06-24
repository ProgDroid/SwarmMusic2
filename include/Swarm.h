/**
 * A swarm object, made up of agents and attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef SWARM_H_
#define SWARM_H_

#include <glm/glm.hpp>
#include <Shader.h>

#include <Agent.h>
#include <Attractor.h>
#include <Triplet.h>

class Swarm
{
private:
    std::vector<Agent> agents;
    std::vector<Attractor> attractors;

    Triplet averagePosition;

    float radiusRepulsion;
    float radiusOrientation;
    float radiusAttraction;

    float blindAngle;
    float speed;
    float maxForce;
    int swarmMode;
public:
    Swarm();

    int getSize() const;
    int getAttractorsCount() const;

    float getRepulsionRadius() const;
    float getOrientationRadius() const;
    float getAttractionRadius() const;

    void setRepulsionRadius(float value);
    void setOrientationRadius(float value);
    void setAttractionRadius(float value);

    float getBlindAngle() const;
    float getSpeed() const;
    float getMaxForce() const;

    void setBlindAngle(float value);
    void setSpeed(float value);
    void setMaxForce(float value);

    int getSwarmMode() const;
    void setSwarmMode(int value);

    Triplet getAveragePosition() const;

    void addAgents();
    void addAttractor(int pitch, unsigned int *VBO, unsigned int *EBO, unsigned int *VAO, int tone);

    void resetAll();
    void resetAttractors();

    void swarm(float deltaTime);
    void setupDrawAgents(unsigned int *VBO, unsigned int *normalVBO, unsigned int *EBO, unsigned int *VAO);
    void setupDrawAttractors(unsigned int *VBO, unsigned int *EBO, unsigned int *VAO);
    void drawAgents(Shader shader);
    void drawAttractors(Shader shader);
};

#endif
