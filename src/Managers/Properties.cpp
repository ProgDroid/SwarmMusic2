#include <Managers/Properties.h>

namespace Managers {

Properties::Properties() {}

Properties::~Properties() {}

int* Properties::getRadiusAttraction() {
    return &radiusAttraction;
}

int* Properties::getRadiusOrientation() {
    return &radiusOrientation;
}

int* Properties::getRadiusRepulsion() {
    return &radiusRepulsion;
}

int* Properties::getBlindAngle() {
    return &blindAngle;
}

float* Properties::getFramerateCap() {
    return &framerateCap;
}

float* Properties::getMaxForce() {
    return &maxForce;
}

float* Properties::getSpeed() {
    return &speed;
}

float* Properties::getTickRate() {
    return &tickRate;
}

}
