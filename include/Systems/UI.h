#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include <Managers/Properties.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include <string>
#include <vector>

namespace Systems {

class UI
{
private:
    struct nk_context* context;
    struct nk_font_atlas* atlas;
    struct nk_glfw glfw = {0};

    // dummy properties to implement UI
    unsigned int style = 2;
    unsigned int swarmMode = 0;
    unsigned int GUIpitch = 0;
    unsigned int scale = 1;
    bool mute = false;

public:
    UI();
    ~UI();

    void init(GLFWwindow* window);

    void update(Managers::Properties* propertiesManager, float fps);

private:
    void drawFpsDisplay(float fps);
    void drawMusicalProperties();
    void drawSwarmProperties(
        int* repulsionRadius,
        int* orientationRadius,
        int* attractionRadius,
        int* blindAngle,
        float* speed,
        float* maxForce
    );
    void muteButton();
    void picker(std::string label, std::vector<std::pair<std::string, unsigned int>> options, unsigned int *variable); // ? might want this to be std::uint8_t?
    void pitchSelector();
    void resetButtonStyle();
    void fslider(std::string label, float min, float max, float* value, float step);
    void islider(std::string label, int min, int max, int* value, int step);
};

}

#endif
