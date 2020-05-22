/**
 * An attracting particle in the swarm to conduct agents
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

/**
 * Random number generator
 *
 * @return float
 */
float attractRand()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 800);

    return (float) distribution(generator);
}

Attractor::Attractor(long pitch, int givenTone)
{
    tone = givenTone;
    mode = SCALES;

    float xCoord  = (float) pitch - 72;
          xCoord *= 800;
          xCoord /= 24;
          xCoord -= 400;

    position = Triplet(xCoord, attractRand() - CUBE_HALF_SIZE, attractRand() - CUBE_HALF_SIZE);

    if (tone == I || tone == V)
    {
        strength = 3;
    }
    else if (tone == ii || tone == IV || tone == vii)
    {
        strength = 2;
    }
    else if (tone == iii || tone == vi || tone == -1)
    {
        strength = 1;
    }

    float modifier = strength == 1 ? -0.2 : (strength == 2 ? 0 : 0.2);

    colour = Triplet(MAUVE_R + modifier, MAUVE_G + modifier, MAUVE_B + modifier);
}

Triplet Attractor::getPosition()
{
    return this->position;
}

/**
 * Draw attractor
 *
 * @return void
 */
void Attractor::draw()
{
    // glColor4f(colour.getX(), colour.getY(), colour.getZ(), 1.0);
    // glPushMatrix();
    // glTranslatef(position.getX(), position.getY(), position.getZ());
    // // glutWireSphere(5, 10, 10);
    // glPopMatrix();
}
