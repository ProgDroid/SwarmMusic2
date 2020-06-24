/**
 * Main class
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shader.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <thread>

#include <Agent.h>
#include <Swarm.h>
#include <Triplet.h>

#include <stk/Plucked.h>
#include <stk/RtAudio.h>
#include <stk/Skini.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

struct TickData {
    stk::Instrmnt *instrument;
    stk::StkFloat frequency;
    long counter;
    bool done;

    TickData()
        : instrument(0), counter(0), done(false) {}
};

const float CENTRAL_C      = 261.63;
const float CUBE_SIZE_HALF = 400.0;
const float OLIVE_BLACK    = 0.23529411764;
const float PI             = 3.14159265;
const float UPDATE_RATE    = 4.0;
const int DOOM             = 0;
const int JAZZ             = 1;
const int POP              = 2;
const int METAL            = 3;
const int PUNK             = 4;
const int MAJOR            = 0;
const int MINOR            = 1;
const int PENTATONIC       = 2;
const int BLUES            = 3;
const int PENTATONIC_SIZE  = 5;
const int BLUES_SIZE       = 6;
const int MAJOR_SIZE       = 7;
const int MINOR_SIZE       = 7;
const int RANDOM           = 0;
const int AVERAGE          = 1;
const int C_MIDI_PITCH     = 72;
const int LENGTH_FACTOR    = 250;

static float repulsionRadius   = 20.0f;
static float orientationRadius = 50.0f;
static float attractionRadius  = 110.0f;
static float blindAngle        = 10.0f;
static float maxForce          = 30.7f;
static float speed             = 30.5f;
static int   scale             = 0;
static int   style             = POP;

static unsigned int GUIpitch = 0;

bool  canExit    = false;
bool  mute       = false;
float deltaTime  = 0.0f;
float deltaSum   = 0.0f;
float fps        = 0.0f;
float lastFrame  = 0.0f;
float theta      = 0.0f;
int   frameCount = 0;

float noteLength, velocity;
int   positionX;
long  pitch;
Swarm swarm;

/**
 * Random number generator
 *
 * @return float
 */
float mainRand() {
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 100);

    return (float) distribution(generator);
}

/**
 * Make a scale starting on a given pitch
 *
 * @param int givenPitch
 *
 * @return void
 */
void makeScale(int givenPitch, unsigned int *VBO, unsigned int *EBO, unsigned int *VAO) {
    swarm.resetAttractors();

    int root      = givenPitch;
    int scaleType = scale;

    // normalise root to make 2 octave scale
    if (root >= 84) {
        root -= 12;
    }

    std::vector<int> intervals;

    // todo scales as objects?
    if (scaleType == MAJOR) {
        intervals.reserve(MAJOR_SIZE);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
    } else if (scaleType == MINOR) {
        intervals.reserve(MINOR_SIZE);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
    } else if (scaleType == PENTATONIC) {
        intervals.reserve(PENTATONIC_SIZE);
        intervals.push_back(3);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(3);
        intervals.push_back(2);
    } else if (scaleType == BLUES) {
        intervals.reserve(BLUES_SIZE);
        intervals.push_back(3);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(1);
        intervals.push_back(3);
        intervals.push_back(2);
    }

    // 2 iterations over the intervals to make 2 octaves scale
    for (int i = 0, size = intervals.size(); i < size * 2; ++i) {
        int index = i % size;
        swarm.addAttractor(root + intervals[index], VBO, EBO, VAO, index);
    }
}

/**
 * Slider creation for UI
 *
 * @param nk_context *context
 * @param const char *label
 * @param float min
 * @param float max
 * @param float *value
 * @param float step
 *
 * @return void
 */
void slider(nk_context *context, const char *label, float min, float max, float *value, float step) {
    // colour bar based on how filled it is
    float ratio = (*value - min) / (max - min);

    int r = ratio * 8 + 38;
    int g = ratio * 126 + 38;
    int b = ratio * 139 + 38;

    context->style.slider.bar_filled = nk_rgb(r, g, b);

    nk_layout_row_begin(context, NK_DYNAMIC, 15, 2);
    {
        nk_layout_row_push(context, 0.40f);
        nk_label(context, label, NK_TEXT_LEFT);
        nk_layout_row_push(context, 0.60f);
        nk_slider_float(context, min, value, max, step);
    }
    nk_layout_row_end(context);
}

/**
 * Draw the UI elements
 *
 * @param nk_glfw *glfw
 * @param nk_context *context
 * @param unsigned int *VBO
 * @param unsigned int *EBO
 * @param unsigned int *VAO
 *
 * @return void
 */
void drawUI(nk_glfw *glfw, nk_context *context, unsigned int *VBO, unsigned int *EBO, unsigned int *VAO) {
    // to control the swarm properties
    context->style.window.fixed_background.data.color.a = 255;
    if (nk_begin(context,
                    "Swarm Properties",
                    nk_rect(0, 0, 285, 155),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR
            )
        ) {
        context->style.slider.cursor_size = nk_vec2(14, 14);

        // repulsion radius
        slider(context, "Repulsion:", 10.0, 40.0, &repulsionRadius, 1.0);

        // orientation radius
        slider(context, "Orientation:", 41.0, 70.0, &orientationRadius, 1.0);

        // attraction radius
        slider(context, "Attraction:", 71.0, 150.0, &attractionRadius, 1.0);

        // blind angle
        slider(context, "Blind Angle:", 0.0, 45.0, &blindAngle, 1.0);

        // speed
        slider(context, "Speed:", 10.5, 63.5, &speed, 0.1);

        // maximum force
        slider(context, "Force:", 10.1, 50.0, &maxForce, 0.1);

        // TODO view angle?
    }
    nk_end(context);

    // to control the musical properties
    if (nk_begin(context,
                    "Musical Properties",
                    nk_rect(glfw->display_width - 285, 0, 285, 245),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR
            )
        ) {
        // style picker
        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, "Style:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(context, 15, 5);
        if (nk_option_label(context, "DOOM",  style == DOOM))  style = DOOM;
        if (nk_option_label(context, "Jazz",  style == JAZZ))  style = JAZZ;
        if (nk_option_label(context, "Pop",   style == POP))   style = POP;
        if (nk_option_label(context, "Metal", style == METAL)) style = METAL;
        if (nk_option_label(context, "Punk",  style == PUNK))  style = PUNK;

        // swarm mode picker
        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, "Swarm Mode:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(context, 15, 2);
        if (nk_option_label(context, "Random",  swarm.getSwarmMode() == RANDOM))  swarm.setSwarmMode(RANDOM);
        if (nk_option_label(context, "Average", swarm.getSwarmMode() == AVERAGE)) swarm.setSwarmMode(AVERAGE);

        // pitch selector
        std::vector<const char *> pitches = {
            "C4", "C#4", "D4", "D#4",
            "E4", "F4", "F#4", "G4",
            "G#4", "A4", "A#4", "B4",
            "C5", "C#5", "D5", "D#5",
            "E5", "F5", "F#5", "G5",
            "G#5", "A5", "A#5", "B5",
            "C6"
        };

        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, "Selected Pitch:", NK_TEXT_LEFT);
        if (nk_combo_begin_label(context, pitches[GUIpitch], nk_vec2(nk_widget_width(context), 197))) {
            nk_layout_row_dynamic(context, 15, 1);
            for (unsigned int i = 0; i < pitches.size(); ++i) {
                // highlight pitch in use
                context->style.contextual_button.normal      = i == GUIpitch ? nk_style_item_color(nk_rgb(255, 90, 95)) : nk_style_item_color(nk_rgb(45, 45, 45));
                context->style.contextual_button.text_normal = i == GUIpitch ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);
                context->style.contextual_button.hover       = i == GUIpitch ? nk_style_item_color(nk_rgb(245, 80, 85)) : nk_style_item_color(nk_rgb(40, 40, 40));
                context->style.contextual_button.text_hover  = i == GUIpitch ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);

                if (nk_combo_item_label(context, pitches[i], NK_TEXT_LEFT)) {
                    GUIpitch = i;
                }
            }
            nk_combo_end(context);
        }

        // scale picker
        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, "Scale:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(context, 15, 4);
        if (nk_option_label(context, "Major",      scale == MAJOR))      scale = MAJOR;
        if (nk_option_label(context, "minor",      scale == MINOR))      scale = MINOR;
        if (nk_option_label(context, "Pentatonic", scale == PENTATONIC)) scale = PENTATONIC;
        if (nk_option_label(context, "Blues",      scale == BLUES))      scale = BLUES;

        // reset style for following buttons
        context->style.button.normal      = nk_style_item_color(nk_rgb(50, 50, 50));
        context->style.button.text_normal = nk_rgb(175, 175, 175);
        context->style.button.hover       = nk_style_item_color(nk_rgb(40, 40, 40));
        context->style.button.text_hover  = nk_rgb(175, 175, 175);

        // add attractor
        nk_layout_row_dynamic(context, 20, 2);
        if (nk_button_label(context, "Add Attractor")) {
            swarm.addAttractor((int) GUIpitch + C_MIDI_PITCH, VBO, EBO, VAO, -1);
        }

        // add scale
        if (nk_button_label(context, "Add Scale")) {
            makeScale((int) GUIpitch + C_MIDI_PITCH, VBO, EBO, VAO);
        }

        // mute
        nk_layout_row_dynamic(context, 25, 1);
        // style button if muted
        context->style.button.normal      = mute ? nk_style_item_color(nk_rgb(255, 90, 95)) : nk_style_item_color(nk_rgb(45, 45, 45)); // 193, 131, 159?
        context->style.button.text_normal = mute ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);
        context->style.button.hover       = mute ? nk_style_item_color(nk_rgb(245, 80, 85)) : nk_style_item_color(nk_rgb(40, 40, 40));
        context->style.button.text_hover  = mute ? nk_rgb(225, 225, 225) : nk_rgb(175, 175, 175);

        if (nk_button_label(context, mute ? "Unmute" : "Mute")) {
            mute = !mute;
        }
    }
    nk_end(context);

    context->style.window.fixed_background.data.color.a = 0;
    // simple fps display
    if (nk_begin(context,
                    "fps",
                    nk_rect((glfw->display_width / 2) - 15, 0, 30, 25),
                    NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR
            )
        ) {
        std::stringstream fpsStream;
        fpsStream << std::fixed << std::setprecision(0) << fps;

        nk_layout_row_dynamic(context, 15, 1);
        nk_label(context, fpsStream.str().c_str(), NK_TEXT_RIGHT);
    }
    nk_end(context);
}

/**
 * Set properties from UI
 *
 * @return void
 */
void setProperties() {
    if (swarm.getRepulsionRadius() != repulsionRadius) {
        swarm.setRepulsionRadius(repulsionRadius);
    }

    if (swarm.getOrientationRadius() != orientationRadius) {
        swarm.setOrientationRadius(orientationRadius);
    }

    if (swarm.getAttractionRadius() != attractionRadius) {
        swarm.setAttractionRadius(attractionRadius);
    }

    if (swarm.getBlindAngle() != blindAngle) {
        swarm.setBlindAngle(blindAngle);
    }

    if (swarm.getSpeed() != speed) {
        swarm.setSpeed(speed);
    }

    if (swarm.getMaxForce() != maxForce) {
        swarm.setMaxForce(maxForce);
    }
}

/**
 * Setup wire cure
 *
 * @return void
 */
void setupWireCube(unsigned int *VBO, unsigned int *EBO, unsigned int *VAO) {
    // cube vertices
    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
    };

    unsigned int indices[] = {
        0, 1,
        1, 3,
        3, 2,
        2, 0,
        0, 4,
        1, 5,
        2, 6,
        3, 7,
        4, 5,
        5, 7,
        7, 6,
        6, 4,
    };

    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);
    glGenVertexArrays(1, VAO);

    glBindVertexArray(*VAO);

    // vertices and colour
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
}

void processInput(GLFWwindow* window) {
    // LEFT
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (theta < 0.0f) {
            theta = 360.0f;
        } else {
            theta -= 10.0f * deltaTime;
        }
    } // LEFT

    // RIGHT
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (theta > 360.0f) {
            theta = 0.0f;
        } else {
            theta += 10.0f * deltaTime;
        }
    } // RIGHT

    // ESCAPE to exit cleanly
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        canExit = true;
    } // ESCAPE

    // PAGE UP to reset app
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        swarm.resetAll();
    }

    // PAGE DOWN to reset attractors only
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        swarm.resetAttractors();
    }
}

int tick(
    void *outputBuffer,
    void *inputBuffer,
    unsigned int nBufferFrames,
    double streamTime,
    RtAudioStreamStatus status,
    void *userData
) {
    TickData *data = reinterpret_cast<TickData *>(userData);

    register stk::StkFloat *samples = (stk::StkFloat *) outputBuffer;

    for (unsigned int i = 0; i < nBufferFrames; ++i) {
        *samples++ = data->instrument->tick();
        if (++data->counter % 2000 == 0) {
            data->instrument->setFrequency(data->frequency);
        }
    }

    // if DOOM or BLUES, 200. if JAZZ or METAL, 100. if PUNK, 50. Default 110
    int lengthFactor = (style == DOOM || style == BLUES) ? 200 : (style == JAZZ || style == METAL) ? 100 : (style == PUNK) ? 50 : 110;

    if (data->counter > noteLength * lengthFactor) {
        data->done = true;
    }

    return 0;
}

/**
 * Compute notes to play
 *
 * @return void
 */
void playMusic() {
    // x coordinate determines pitch between C4=72 and C6=96,
    // for a range of 2 octaves
    pitch = ((swarm.getAveragePosition().getX() + CUBE_SIZE_HALF) * 24) / (CUBE_SIZE_HALF * 2);
    if (pitch < 0) {
        pitch = 0;
    } else if (pitch > 24) {
        pitch = 24;
    }
    pitch += 72;

    // y coordinate determines velocity ("volume" at which individual note is played)
    // 0.0 to 1.0
    velocity = ((swarm.getAveragePosition().getY() + CUBE_SIZE_HALF)) / (CUBE_SIZE_HALF * 2);
    if (velocity < 0) {
        velocity = 0;
    } else if (velocity > 1.0) {
        velocity = 1.0;
    }

    // z coordinate determines note length in ms
    noteLength = ((swarm.getAveragePosition().getZ() + CUBE_SIZE_HALF)) / (CUBE_SIZE_HALF * 2);
    noteLength = noteLength * LENGTH_FACTOR * deltaTime;
}

/**
 * The music thread
 *
 * @return void
 */
void music() {
    stk::Stk::setSampleRate(44100.0);
    stk::Stk::setRawwavePath("./stk-4.6.0/rawwaves");

    TickData data;
    RtAudio dac;

    RtAudio::StreamParameters parameters;
    
    parameters.deviceId  = dac.getDefaultOutputDevice();
    parameters.nChannels = 1;

    RtAudioFormat format           = (sizeof(stk::StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned      int bufferFrames = stk::RT_BUFFER_SIZE;

    try {
        dac.openStream(&parameters, NULL, format, (unsigned int) stk::Stk::sampleRate(), &bufferFrames, &tick, (void *) &data);
    } catch (RtAudioError &error) {
        error.printMessage();
        return;
    }

    try {
        data.instrument = new stk::Plucked(CENTRAL_C);
    } catch (stk::StkError &) {
        delete data.instrument;
        return;
    }

    try {
        dac.startStream();
    } catch (RtAudioError &error) {
        error.printMessage();
        delete data.instrument;
        return;
    }

    while(!canExit) {
        data.frequency = stk::Midi2Pitch[pitch];
        if (mainRand() < (float) style && !mute) {
            data.instrument->noteOn(data.frequency, velocity);
        }

        while(!data.done) {
            stk::Stk::sleep(noteLength);
        }

        data.counter = 0;
        data.done    = false;
    }

    try {
        dac.closeStream();
    } catch (RtAudioError &error) {
        error.printMessage();
    }

    delete data.instrument;
}

/**
 * Error printing callback
 * 
 * @param int error
 * @param const char* description
 *
 * @return void
 */
void errorCallback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

/**
 * Frame buffer size callback (called when resizing window)
 * 
 * @param GLFWwindow* window
 * @param int width
 * @param int height
 *
 * @return void
 */
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    static GLFWwindow *window;
    int width = 0, height = 0;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Swarm Music", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return EXIT_FAILURE;
    }
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    float cap = 1.0f / glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;

    Shader shader("./shaders/test.vs", "./shaders/test.fs");
    Shader shaderLight("./shaders/light.vs", "./shaders/light.fs");
    Shader shaderLightSource("./shaders/lightSource.vs", "./shaders/lightSource.fs");

    // UI
    struct nk_glfw glfw = {0};
    struct nk_context* context = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
	struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);

    unsigned int VBO, EBO, VAO;
    setupWireCube(&VBO, &EBO, &VAO);
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::scale(cubeModel, glm::vec3(400.0f, 400.0f, 400.0f));

    unsigned int lightVBO, lightEBO, lightVAO;
    // light source cube
    setupWireCube(&lightVBO, &lightEBO, &lightVAO);
    glm::mat4 lightModel  = glm::mat4(1.0f);
    glm::vec3 lightPos    = glm::vec3(0.0f, 420.0f, 0.0f);

    float lightAmbient  = 0.2f;
    float lightDiffuse  = 0.75f;
    float lightSpecular = 1.0f;

    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    unsigned int agentVBO, agentNormalVBO, agentEBO, agentVAO;
    swarm.setupDrawAgents(&agentVBO, &agentNormalVBO, &agentEBO, &agentVAO);

    unsigned int attractorVBO, attractorEBO, attractorVAO;

    std::thread soundThread(music);
    soundThread.detach();

	while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        deltaSum += deltaTime;
        frameCount++;
        lastFrame = currentFrame;

        if (deltaSum > 1.0 / UPDATE_RATE) {
            fps = frameCount / deltaSum;
            frameCount = 0;
            deltaSum   = 0;
        }

        glfwGetWindowSize(window, &width, &height);

        glEnable(GL_DEPTH_TEST);
        processInput(window);

		nk_glfw3_new_frame(&glfw); 
        drawUI(&glfw, context, &attractorVBO, &attractorEBO, &attractorVAO);

		glClearColor(OLIVE_BLACK, OLIVE_BLACK, OLIVE_BLACK, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(glm::vec3(1500.0f * sin(theta * (PI / 180.0f)), 500.0f, 1500.0f * cos(theta * (PI / 180.0f))),
                            glm::vec3(0.0f, -80.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 2500.0f);

        shaderLightSource.use();
        shaderLightSource.setMat4("model", lightModel);
        shaderLightSource.setMat4("view", view);
        shaderLightSource.setMat4("projection", projection);

        glBindVertexArray(lightVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

        shaderLight.use();
        shaderLight.setMat4("view", view);
        shaderLight.setMat4("projection", projection);

        // for wire cube
        shaderLight.setVec3("light.ambient",  glm::vec3(1.0f, 1.0f, 1.0f));
        shaderLight.setVec3("light.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
        shaderLight.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shaderLight.setVec3("light.position", lightPos);

        // draw cube
        glBindVertexArray(VAO);

        shaderLight.setMat4("model", cubeModel);
        shaderLight.setVec3("material.ambient",  glm::vec3(0.02f, 0.02f, 0.02f));
        shaderLight.setVec3("material.diffuse",  glm::vec3(0.01f, 0.01f, 0.01f));
        shaderLight.setVec3("material.specular", glm::vec3(0.4f, 0.4f, 0.4f));
        shaderLight.setFloat("material.shininess", 0.078125f * 128);

        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

        // for agents and attractors
        shaderLight.setVec3("light.ambient",  glm::vec3(lightAmbient, lightAmbient, lightAmbient));
        shaderLight.setVec3("light.diffuse",  glm::vec3(lightDiffuse, lightDiffuse, lightDiffuse));
        shaderLight.setVec3("light.specular", glm::vec3(lightSpecular, lightSpecular, lightSpecular));

        // draw agents
        glBindVertexArray(agentVAO);
        swarm.drawAgents(shaderLight);

        glBindVertexArray(attractorVAO);
        swarm.drawAttractors(shaderLight);

        glBindVertexArray(0);

		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

		glfwSwapBuffers(window);
		glfwPollEvents();

        setProperties();
        swarm.swarm(deltaTime);
        playMusic();

        while (glfwGetTime() < lastFrame + cap) {
            // do nothing
        }
	}

    if (soundThread.joinable()) {
        soundThread.join();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);

    glDeleteVertexArrays(1, &agentVAO);
    glDeleteBuffers(1, &agentEBO);
    glDeleteBuffers(1, &agentNormalVBO);
    glDeleteBuffers(1, &agentVBO);

    glDeleteVertexArrays(1, &attractorVAO);
    glDeleteBuffers(1, &attractorEBO);
    glDeleteBuffers(1, &attractorVBO);

	nk_glfw3_shutdown(&glfw);
	glfwTerminate();
    return EXIT_SUCCESS;
}
