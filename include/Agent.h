/**
 * An agent of the swarm, with its local rules for movement
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef AGENT_H_
#define AGENT_H_

#include <glm/glm.hpp>

#include <vector>

#include <Attractor.h>
#include <Triplet.h>

const float CUBE_HALF_SIZE = 400.0;
const float BLUE_R         = 0.03137254901; // 8, 126, 139
const float BLUE_G         = 0.49411764705;
const float BLUE_B         = 0.54509803921;
const float RED_R          = 1.0;           // 255, 90, 95
const float RED_G          = 0.35294117647;
const float RED_B          = 0.3725490196;
const float DEFAULT_WHITE  = 0.96078431372;

class Agent
{
private:
    Triplet position;  // random
    Triplet direction; // also random, always needs to be normalised

    Triplet acceleration = Triplet(0.0f, 0.0f, 0.0f);
    Triplet BLUE         = Triplet(BLUE_R, BLUE_G, BLUE_B);
    Triplet RED          = Triplet(RED_R, RED_G, RED_B);
    Triplet WHITE        = Triplet(DEFAULT_WHITE, DEFAULT_WHITE, DEFAULT_WHITE);
    Triplet colour       = Triplet(0.0f, 0.0f, 0.0f);
    Triplet oldColour    = Triplet(0.0f, 0.0f, 0.0f);
    Triplet MIN          = Triplet(-CUBE_HALF_SIZE, -CUBE_HALF_SIZE, -CUBE_HALF_SIZE);
    Triplet MAX          = Triplet(CUBE_HALF_SIZE, CUBE_HALF_SIZE, CUBE_HALF_SIZE);

    int colourSwapTime   = 0;

    static glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
public:
    Agent();

    Triplet getPosition() const;
    Triplet getColour() const;
    Triplet getOldColour() const;
    void setOldColour(Triplet value);

    int getColourSwapTime() const;
    void setColourSwapTime(int value);

    void setColour(int colourCount);
    void computeChange(Triplet newDirection, float count, Triplet direction, float maxForce) const;

    Triplet repulsion(std::vector<Agent> &agents, float radiusRepulsion,float blindAngle, float maxForce);
    Triplet orientation(std::vector<Agent> &agents, float radiusRepulsion, float radiusOrientation, float blindAngle, float maxForce);
    Triplet attraction(std::vector<Agent> &agents, float radiusOrientation, float radiusAttraction, float blindAngle, float maxForce);
    Triplet bounding() const;

    static void setupDraw(unsigned int *VBO, unsigned int *normalVBO, unsigned int *EBO, unsigned int *VAO);
    void transform(glm::mat4 *agentModel);
    static void draw();
    void move(float speed, std::vector<Attractor> attractors, float deltaTime);
    void step(std::vector<Agent> &agents, float radiusRepulsion, float radiusOrientation, float radiusAttraction, float angle, float maxForce);
};

#endif
