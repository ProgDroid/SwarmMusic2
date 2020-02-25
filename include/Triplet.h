/**
 * A container for a 3 dimensional point with helper calculation functions
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#ifndef TRIPLET_H_
#define TRIPLET_H_

class Triplet
{
private:
    float x, y, z;

public:
    Triplet();
    Triplet(float a, float b, float c);
    
    float getX();
    float getY();
    float getZ();

    void setX(float X);
    void setY(float Y);
    void setZ(float Z);

    float length();

    Triplet operator+(Triplet v);
    Triplet operator-(Triplet v);
    float operator*(Triplet v);

    void scalarDiv(float scalar);
    void scalarMul(float scalar);
    void normalise();

    float distance(Triplet b);
    float angle(Triplet b);
};

#endif
