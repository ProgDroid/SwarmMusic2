/**
 * A swarm object, made up of agents and attractors
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

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
float swarmRand()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 20);

    return (float)distribution(generator);
}

Swarm::Swarm()
{
    agents.reserve(MAX_SIZE);
    attractors.reserve(MAX_ATTRACTORS);
    addAgents();

    radiusRepulsion   = 20.0;
    radiusOrientation = 50.0;
    radiusAttraction  = 80.0;
    blindAngle        = 10.0;
    speed             = 1.5;
    maxForce          = 0.7;
    swarmMode         = RANDOM;

    Triplet averagePosition(0.0, 0.0, 0.0);
}

int Swarm::getSize()
{
    return this->agents.size();
}

int Swarm::getAttractorsCount()
{
    return this->attractors.size();
}

float Swarm::getRepulsionRadius()
{
    return this->radiusRepulsion;
}

float Swarm::getOrientationRadius()
{
    return this->radiusOrientation;
}

float Swarm::getAttractionRadius()
{
    return this->radiusAttraction;
}

void Swarm::setRepulsionRadius(float value)
{
    this->radiusRepulsion = value;
}

void Swarm::setOrientationRadius(float value)
{
    this->radiusOrientation = value;
}

void Swarm::setAttractionRadius(float value)
{
    this->radiusAttraction = value;
}

float Swarm::getBlindAngle()
{
    return this->blindAngle;
}

float Swarm::getSpeed()
{
    return this->speed;
}

float Swarm::getMaxForce()
{
    return this->maxForce;
}

void Swarm::setBlindAngle(float value)
{
    this->blindAngle = value;
}

void Swarm::setSpeed(float value)
{
    this->speed = value;
}

void Swarm::setMaxForce(float value)
{
    this->maxForce = value;
}

int Swarm::getSwarmMode()
{
    return this->swarmMode;
}

void Swarm::setSwarmMode(int value)
{
    this->swarmMode = value;
}

Triplet Swarm::getAveragePosition()
{
    return this->averagePosition;
}

void Swarm::addAgents()
{
    for (int i = 0; i < MAX_SIZE; ++i)
    {
        Agent agent;
        agents.push_back(agent);
    }
}

void Swarm::resetAll()
{
    agents.erase(agents.begin(), agents.end());

    for (int i = 0; i < MAX_SIZE; ++i)
    {
        Agent agent;
        agents.push_back(agent);
    }
}

void Swarm::resetAttractors()
{
    attractors.erase(attractors.begin(), attractors.end());
}


void Swarm::addAttractor(int pitch, int tone = -1)
{
    Attractor attractor(pitch, tone);
    attractors.push_back(attractor);
}

void Swarm::swarm()
{
    for (int i = 0, size = getSize(); i < size; ++i)
    {
        agents[i].step(agents, radiusRepulsion, radiusOrientation, radiusAttraction, blindAngle, maxForce);

        float positionX = swarmMode == AVERAGE ? averagePosition.getX() + agents[i].getPosition().getX() : agents[i].getPosition().getX();
        float positionY = swarmMode == AVERAGE ? averagePosition.getY() + agents[i].getPosition().getY() : agents[i].getPosition().getY();
        float positionZ = swarmMode == AVERAGE ? averagePosition.getZ() + agents[i].getPosition().getZ() : agents[i].getPosition().getZ();

        if (swarmMode == RANDOM)
        {
            if (swarmRand() < 2)
            {
                averagePosition.setX(positionX);
            }
            if (swarmRand() < 2)
            {
                averagePosition.setY(positionY);
            }
            if (swarmRand() < 2)
            {
                averagePosition.setZ(positionZ);
            }

            continue;
        }

        averagePosition.setX(positionX);
        averagePosition.setY(positionY);
        averagePosition.setZ(positionZ);
    }

    for (int i = 0, size = getSize(); i < size; ++i)
    {
        agents[i].move(speed, attractors);
    }

    if (swarmMode == AVERAGE)
    {
        averagePosition.scalarDiv(getSize());
    }
}

void Swarm::drawAgents()
{
    for (int i = 0, size = getSize(); i < size; ++i)
    {
        agents.at(i).draw();
    }
}

void Swarm::drawAttractors()
{
    if (getAttractorsCount() == 0)
    {
        return;
    }

    for (int i = 0, size = getAttractorsCount(); i < size; ++i)
    {
        attractors.at(i).draw();
    }
}
