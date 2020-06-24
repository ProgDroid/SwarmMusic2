/**
 * An attracting particle in the swarm to conduct agents
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef ATTRACTOR_H_
#define ATTRACTOR_H_

#include <glm/glm.hpp>

#include <Triplet.h>

class Attractor
{
private:
    Triplet position;
    Triplet colour;

    int tone;
    int strength;
    int mode;

    static Triplet initPosition(int pitch);
public:
    Attractor(int pitch, int givenTone);

    Triplet getPosition() const;
    Triplet getColour() const;

    static void setupDraw(unsigned int *VBO, unsigned int *EBO, unsigned int *VAO);
    void transform(glm::mat4 *attractorModel);
    static void draw();
};

#endif
