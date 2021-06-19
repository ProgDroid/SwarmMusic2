#ifndef PROPERTIES_MANAGER_H
#define PROPERTIES_MANAGER_H

#include <Consts.h>

#include <cstdint>

namespace Managers {

class Properties
{
private: 
    int radiusAttraction  = RADIUS_ATTRACTION;
    int radiusOrientation = RADIUS_ORIENTATION;
    int radiusRepulsion   = RADIUS_REPULSION;
    int blindAngle        = BLIND_ANGLE;

    float framerateCap = FRAMERATE_CAP;
    float maxForce     = MAX_FORCE;
    float speed        = SPEED;
    float tickRate     = TICK_RATE;

    // bit field if there's more? //!window manager?
    bool capFramerate = true;

public:
    Properties();
    ~Properties();

    int* getRadiusAttraction();
    int* getRadiusOrientation();
    int* getRadiusRepulsion();
    int* getBlindAngle();

    float* getFramerateCap();
    float* getMaxForce();
    float* getSpeed();
    float* getTickRate();
};

}

#endif
