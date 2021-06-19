#ifndef CONSTS_H
#define CONSTS_H

#include <cstdint>
#include <string>

extern const size_t       CUBE_SIZE;
extern const size_t       CUBE_SIZE_HALF;
extern const size_t       MAX_AGENTS;
extern const size_t       MAX_ATTRACTORS;
extern const size_t       MAX_ENTITIES;

extern const std::uint8_t AGENT;
extern const std::uint8_t ATTRACTOR;

extern const int          BLIND_ANGLE;
extern const int          RADIUS_REPULSION;
extern const int          RADIUS_ORIENTATION;
extern const int          RADIUS_ATTRACTION;

extern const float        FPS_UPDATE_RATE;
extern const float        FRAMERATE_CAP;
extern const float        MAX_FORCE;
extern const float        PI;
extern const float        SPEED;
extern const float        TICK_RATE;

extern const std::string  COMPONENTS_DATA_PATH;
extern const std::string  MESH_DATA_PATH;
extern const std::string  SHADERS_PATH;

#endif
