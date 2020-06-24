/**
 * A container for a 3 dimensional point with helper calculation functions
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <cmath>

#include <Triplet.h>

const float TO_DEGREES = 57.295779524f;

Triplet::Triplet() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Triplet::Triplet(float X, float Y, float Z) {
    x = X;
    y = Y;
    z = Z;
}

float Triplet::getX() const {
    return this->x;
}

float Triplet::getY() const {
    return this->y;
}

float Triplet::getZ() const {
    return this->z;
}

void Triplet::setX(float value) {
    x = value;
}

void Triplet::setY(float value) {
    y = value;
}

void Triplet::setZ(float value) {
    z = value;
}

/**
 * Length of a vector from the origin to a given point
 *
 * @return float
 */
float Triplet::length() const {
    float length = sqrt((x * x) + (y * y) + (z * z));
    return length;
}

/**
 * Vector addition
 *
 * @param Triplet vector
 * @return Triplet
 */
Triplet Triplet::operator+(Triplet vector) {
    Triplet triplet = Triplet((x + vector.x), (y + vector.y), (z + vector.z));
    return triplet;
}

/**
 * Vector subtraction
 *
 * @param Triplet vector
 * @return Triplet
 */
Triplet Triplet::operator-(Triplet vector) {
    Triplet triplet = Triplet((x - vector.x), (y - vector.y), (z - vector.z));
    return triplet;
}

/**
 * Calculate dot product
 *
 * @param Triplet vector
 * @return float
 */
float Triplet::operator*(Triplet vector) const {
    return (x * vector.x) + (y * vector.y) + (z * vector.z);
}

/**
 * Divide by a scalar
 *
 * @return void
 */
void Triplet::scalarDiv(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
}

/**
 * Multiply by a scalar
 *
 * @return void
 */
void Triplet::scalarMul(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

/**
 * Normalise vector
 *
 * This truncates the vector coordinates to values between 0 and 1
 *
 * @return void
 */
void Triplet::normalise() {
    float magnitude = length();

    if (magnitude > 0) {
        scalarDiv(magnitude);
    }
}

/**
 * Calculate distance from this to another point
 *
 * @param Triplet b
 * @return float
 */
float Triplet::distance(Triplet b) const {
    Triplet distVector = b - *this;
    return distVector.length();
}

/**
 * Compute angle between the vectors represented by two points
 *
 * @param Triplet b
 * @return float
 */
float Triplet::angle(Triplet b) const {
    float magnitude  = length();
    float magnitudeB = b.length();

    if (magnitude > 0 && magnitudeB > 0) {
        float dotProduct = *this * b;
        float cosTheta = dotProduct / (magnitude * magnitudeB);

        if (cosTheta <= -1) {
            return 180.0f;
        } else if (cosTheta >= 1) {
            return 0.0f;
        }

        return acos(cosTheta) * TO_DEGREES;
    }

    return 0.0f;
}
