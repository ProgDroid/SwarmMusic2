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
    Triplet(float X, float Y, float Z);
    
    float getX() const;
    float getY() const;
    float getZ() const;

    void setX(float value);
    void setY(float value);
    void setZ(float value);

    float length() const;

    Triplet operator+(Triplet vector);
    Triplet operator-(Triplet vector);
    float operator*(Triplet vector) const;

    void scalarDiv(float scalar);
    void scalarMul(float scalar);
    void normalise();

    float distance(Triplet b) const;
    float angle(Triplet b) const;
};

#endif
