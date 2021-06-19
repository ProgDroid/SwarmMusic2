#include <Consts.h>

#include <string>

// ! changing the maximums here means changing the value in Entity.h
const size_t       CUBE_SIZE            = 800;
const size_t       CUBE_SIZE_HALF       = 400;
const size_t       MAX_AGENTS           = 500;
const size_t       MAX_ATTRACTORS       = 13;
const size_t       MAX_ENTITIES         = MAX_AGENTS + MAX_ATTRACTORS;

const std::uint8_t AGENT                = 1;
const std::uint8_t ATTRACTOR            = 2;

const int          BLIND_ANGLE          = 10;
const int          RADIUS_REPULSION     = 20;
const int          RADIUS_ORIENTATION   = 50;
const int          RADIUS_ATTRACTION    = 110;

const float        FPS_UPDATE_RATE      = 1.0f / 10.0f;
const float        FRAMERATE_CAP        = 1.0f / 60.0f;
const float        MAX_FORCE            = 30.7f;
const float        PI                   = 3.14159265;
const float        SPEED                = 30.5f;
const float        TICK_RATE            = 1.0f / 60.0f;

const std::string  COMPONENTS_DATA_PATH = "data/componentsData.json";
const std::string  MESH_DATA_PATH       = "data/meshData.json";
const std::string  SHADERS_PATH         = "shaders/";
