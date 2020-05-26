/**
 * An agent of the swarm, with its local rules for movement
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include <Agent.h>
#include <Attractor.h>
#include <Triplet.h>

const float TO_DEGREES   = 57.295779524;
const float MIN_DISTANCE = 100000000;
const float BOUNDARY     = 1.0;

/**
 * Random number generator
 *
 * Generates numbers from 0 to CUBE_HALF_SIZE * 2 which can then be used for the position coordinates
 * directly by adjusting to -CUBE_HALF_SIZE to CUBE_HALF_SIZE
 * Also used for the direction, adjusting similarly and then normalising
 *
 * @return float
 */
float agentRand()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, CUBE_HALF_SIZE * 2);

    return (float) distribution(generator);
}

Agent::Agent()
{
    // assign random colour
    float randomValue = agentRand();

    colour = randomValue >= 0 && randomValue <= (CUBE_HALF_SIZE * 2) ? (randomValue <= CUBE_HALF_SIZE ? BLUE : RED) : WHITE;

    position  = Triplet(agentRand() - CUBE_HALF_SIZE, agentRand() - CUBE_HALF_SIZE, agentRand() - CUBE_HALF_SIZE);
    direction = Triplet((agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE, (agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE, (agentRand() - CUBE_HALF_SIZE) / CUBE_HALF_SIZE);
}

Triplet Agent::getPosition()
{
    return this->position;
}

/**
 * Change colour of an agent if orientation area neighbours are mostly of the other colour
 *
 * @param int colourCount Positive (+) value for blue, negative (-) for red
 * @return void
 */
void Agent::setColour(int colourCount)
{
    colour = colourCount > 0 ? BLUE : RED;
}

void Agent::computeChange(Triplet newDirection, float count, Triplet direction, float maxForce)
{
    newDirection.scalarDiv(count);
    newDirection.normalise();
    newDirection = newDirection - direction;
    // truncate to maximum force
    if (newDirection.length() > maxForce)
    {
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
    // std::cout << agents[499].getPosition().getX() << std::endl;
    Triplet newDirection(0.0, 0.0, 0.0);
    int count = 0;

    for (std::size_t i = 0, size = agents.size(); i < size; ++i)
    {
        // calculate distance to another agent
        float distance = position.distance(agents.at(i).position);
        // if not itself and within radius
        if (distance > 0 && distance <= radiusRepulsion)
        {
            // check if not behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle))
            {
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
    if (count > 0)
    {
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

    for (std::size_t i = 0, size = agents.size(); i < size; ++i)
    {
        // calculate distance
        float distance = position.distance(agents.at(i).position);
        // if within range (also not itself)
        if (distance > radiusRepulsion && distance <= radiusOrientation)
        {
            // check if behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle))
            {
                newDirection = newDirection + agents.at(i).direction;
                ++count;

                // check colour R value
                // if 1.0 it is red so take 1, else add 1 (is blue)
                if (agents.at(i).colour.getX() == 1.0)
                {
                    --colourCount;
                }
                else
                {
                    ++colourCount;
                }
            }
        }
    }

    if (count > 0)
    {
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

    for (std::size_t i = 0, size = agents.size(); i < size; ++i)
    {
        // calculate distance
        float distance = position.distance(agents.at(i).position);
        // if within range
        if (distance > radiusOrientation && distance <= radiusAttraction)
        {
            // check if behind
            Triplet vectorBetweenPoints = agents.at(i).position - position;
            float   angle               = direction.angle(vectorBetweenPoints);
            if (angle <= (180 - blindAngle))
            {
                newDirection = newDirection + vectorBetweenPoints;
                ++count;
            }
        }
    }

    if (count > 0)
    {
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
Triplet Agent::bounding()
{
    Triplet vector(0.0, 0.0, 0.0);

    if (position.getX() <= MIN.getX())
    {
        vector.setX(BOUNDARY);
    }
    else if (position.getX() >= MAX.getX())
    {
        vector.setX(-BOUNDARY);
    }

    if (position.getY() <= MIN.getY())
    {
        vector.setY(BOUNDARY);
    }
    else if (position.getY() >= MAX.getY())
    {
        vector.setY(-BOUNDARY);
    }

    if (position.getZ() <= MIN.getZ())
    {
        vector.setZ(BOUNDARY);
    }
    else if (position.getZ() >= MAX.getZ())
    {
        vector.setZ(-BOUNDARY);
    }

    return vector;
}

/**
 * Draw an agent
 *
 *
 * @return void
 */
void Agent::draw()
{
    // glColor4f(colour.getX(), colour.getY(), colour.getZ(), 1.0);
    // glPushMatrix();
    // glTranslatef(position.getX(), position.getY(), position.getZ());
    // Triplet tempDirection = Triplet(direction.getX(), direction.getY(), direction.getZ());
    // tempDirection.scalarMul(TO_DEGREES);
    // glRotatef(tempDirection.getX(), 1.0, 0.0, 0.0);
    // glRotatef(tempDirection.getY(), 0.0, cos(direction.getX()), -sin(direction.getX()));
    // glRotatef(tempDirection.getZ(), 0.0, 0.0, 1.0);
    // // scaling for looks
    // glScalef(1.0, 0.25, 0.25);
    // glutSolidCone(5.5, 5.5, 5, 1);
    // glPopMatrix();
}

/**
 * Move agent
 *
 * @param float speed
 * @param std::vector<Attractor> attractors
 * @return void
 */
void Agent::move(float speed, std::vector<Attractor> attractors)
{
    if (attractors.size() != 0)
    {
        float  minDistance = MIN_DISTANCE;
        size_t index       = 0;

        for (std::size_t i = 0, size = attractors.size(); i < size; ++i)
        {
            float distance = position.distance(attractors.at(i).getPosition());
            if (distance < minDistance)
            {
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
    if (orientationVector.length() != 0 && attractionVector.length() != 0)
    {
        tmpVector.scalarDiv(2);
    }

    Triplet boundingVector = bounding();

    acceleration = acceleration + tmpVector + boundingVector;
}
