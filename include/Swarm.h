/**
 * A swarm object, made up of agents and attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef SWARM_H_
#define SWARM_H_

#include <Agent.h>
#include <Attractor.h>
#include <Triplet.h>

class Swarm {
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

    int getSize();
    int getAttractorsCount();

    float getRepulsionRadius();
    float getOrientationRadius();
    float getAttractionRadius();

    void setRepulsionRadius(float value);
    void setOrientationRadius(float value);
    void setAttractionRadius(float value);

    float getBlindAngle();
    float getSpeed();
    float getMaxForce();

    void setBlindAngle(float value);
    void setSpeed(float value);
    void setMaxForce(float value);

    int getSwarmMode();
    void setSwarmMode(int value);

    Triplet getAveragePosition();

    void addAgents();
    void addAttractor(long pitch, int tone);

    void resetAll();
    void resetAttractors();

    void swarm();
    void drawAgents();
    void drawAttractors();
};

#endif
