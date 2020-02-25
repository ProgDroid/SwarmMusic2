/**
 * An attracting particle in the swarm to conduct agents
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef ATTRACTOR_H_
#define ATTRACTOR_H_

#include <Triplet.h>

class Attractor {
private:
    Triplet position;
    Triplet colour;

    int tone;
    int strength;
    int mode;

public:
    Attractor(long pitch, int givenTone);

    Triplet getPosition();

    void draw();
};

#endif
