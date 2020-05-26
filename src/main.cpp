/**
 * Main class
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <random>
#include <thread>

#include <Agent.h>
#include <Swarm.h>
#include <Triplet.h>

#include <stk/Plucked.h>
#include <stk/RtAudio.h>
#include <stk/Skini.h>

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

struct TickData
{
    stk::Instrmnt *instrument;
    stk::StkFloat frequency;
    long counter;
    bool done;

    TickData()
        : instrument(0), counter(0), done(false) {}
};

const float CENTRAL_C      = 261.63;
const float CUBE_SIZE_HALF = 400.0;
const float OLIVEBLACK     = 0.23529411764;
const float PI             = 3.14159265;
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

static float repulsionRadius   = 20.0;
static float orientationRadius = 50.0;
static float attractionRadius  = 110.0;
static float blindAngle        = 10;
static float maxForce          = 0.7;
static float speed             = 1.5;
static int   scale             = 0;
static int   style             = POP;

static unsigned int GUIpitch = 0;

bool  canExit  = false;
bool  mute     = false;
float theta    = 0.0;

float noteLength, velocity;
int   positionX;
long  pitch;
Swarm swarm;

/**
 * Random number generator
 */
float mainRand()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, 100);

    return (float) distribution(generator);
}

void makeScale(int givenPitch) {
    swarm.resetAttractors();

    int root      = givenPitch;
    int scaleType = scale;

    // normalise root to make 2 octave scale
    if (root >= 84)
    {
        root -= 12;
    }

    std::vector<int> intervals;

    // todo scales as objects?
    if (scaleType == MAJOR)
    {
        intervals.reserve(MAJOR_SIZE);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
    }
    else if (scaleType == MINOR)
    {
        intervals.reserve(MINOR_SIZE);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(2);
        intervals.push_back(2);
    }
    else if (scaleType == PENTATONIC)
    {
        intervals.reserve(PENTATONIC_SIZE);
        intervals.push_back(3);
        intervals.push_back(2);
        intervals.push_back(2);
        intervals.push_back(3);
        intervals.push_back(2);
    }
    else if (scaleType == BLUES)
    {
        intervals.reserve(BLUES_SIZE);
        intervals.push_back(3);
        intervals.push_back(2);
        intervals.push_back(1);
        intervals.push_back(1);
        intervals.push_back(3);
        intervals.push_back(2);
    }

    // 2 iterations over the intervals to make 2 octaves scale
    for (int i = 0, size = intervals.size(); i < size * 2; ++i)
    {
        int index = i % size;
        swarm.addAttractor(root + intervals[index], index);
    }
}

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
 * @return void
 */
void drawUI(nk_glfw *glfw, nk_context *context) {
    // to control the swarm properties
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
        slider(context, "Speed:", 0.5, 3.5, &speed, 0.1);

        // maximum force
        slider(context, "Force:", 0.1, 2.0, &maxForce, 0.1);

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
            swarm.addAttractor((int) GUIpitch + C_MIDI_PITCH, -1);
        }

        // add scale
        if (nk_button_label(context, "Add Scale")) {
            makeScale((int) GUIpitch + C_MIDI_PITCH);
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
}

void drawWireCube()
{
    // glColor4f(0.0, 0.0, 0.0, 1.0);
    // // glPushMatrix();
    // // glMatrixMode(GL_MODELVIEW);
    // // glLoadIdentity();
    // glBegin(GL_QUADS);
    //     glVertex3f(-1.0f, -1.0f, 1.0f);
    //     glVertex3f(-1.0f, -1.0f, -1.0f);
    //     glVertex3f(1.0f, -1.0f, -1.0f);
    //     glVertex3f(1.0f, -1.0f, 1.0f);
    // glEnd();
    // glPopMatrix();
    // glColor4f(0.0, 0.0, 0.0, 1.0);
    // glPushMatrix();
    // glutWireCube(800.0);
    // glPopMatrix();
}

void render(GLFWwindow* window)
{
    // glPushMatrix();
    // glLoadIdentity();
    // gluLookAt(1250 * sin(theta * (PI / 180)), 500.0, 1250 * cos(theta * (PI / 180)),
    //           0.0, -100.0, 0.0,
    //           0.0, 1.0, 0.0);
    // glPopMatrix();

    glClear(GL_COLOR_BUFFER_BIT);

    swarm.drawAgents();
    // drawWireCube();
    // swarm.drawAttractors();

    // draw UI
    // TwDraw();
}

void reshape(GLFWwindow* window, int width, int height)
{
    // glClearColor(OLIVEBLACK, OLIVEBLACK, OLIVEBLACK, 1.0);
    // glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    // // glMatrixMode(GL_PROJECTION);
    // // glLoadIdentity();
    // // gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 10000.0);
    // // glMatrixMode(GL_MODELVIEW);
    // TwWindowSize(width, height);
    // positionX = width - 200;
}

void idle()
{
    // glutPostRedisplay();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        if (theta <= 0.0)
        {
            theta = 359.5;
        }
        else
        {
            theta -= 0.5;
        }
    } // LEFT

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        if (theta >= 359.5)
        {
            theta = 0.0;
        }
        else
        {
            theta += 0.5;
        }
    } // RIGHT

    // to exit cleanly
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        canExit = true;
    } // HOME

    if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
    {
        swarm.resetAll();
    }

    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
    {
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

    for (unsigned int i = 0; i < nBufferFrames; ++i)
    {
        *samples++ = data->instrument->tick();
        if (++data->counter % 2000 == 0)
        {
            data->instrument->setFrequency(data->frequency);
        }
    }

    int lengthFactor = 110;
    if (style == DOOM || style == BLUES)
    {
        lengthFactor = 200;
    }
    else if (style == JAZZ || style == METAL)
    {
        lengthFactor = 100;
    }
    else if (style == PUNK)
    {
        lengthFactor = 50;
    }

    if (data->counter > noteLength * lengthFactor)
    {
        data->done = true;
    }

    return 0;
}

void playMusic()
{
    // x coordinate determines pitch between C4=72 and C6=96,
    // for a range of 2 octaves
    pitch = ((swarm.getAveragePosition().getX() + CUBE_SIZE_HALF) * 24) / (CUBE_SIZE_HALF * 2);
    if (pitch < 0)
    {
        pitch = 0;
    }
    else if (pitch > 24)
    {
        pitch = 24;
    }
    pitch += 72;

    // y coordinate determines velocity ("volume" at which individual note is played)
    // 0.0 to 1.0
    velocity = ((swarm.getAveragePosition().getY() + CUBE_SIZE_HALF)) / (CUBE_SIZE_HALF * 2);
    if (velocity < 0)
    {
        velocity = 0;
    }
    else if (velocity > 1.0)
    {
        velocity = 1.0;
    }

    // z coordinate determines note length in ms
    noteLength = ((swarm.getAveragePosition().getZ() + CUBE_SIZE_HALF)) / (CUBE_SIZE_HALF * 2);
    noteLength = noteLength * LENGTH_FACTOR;
}

void music()
{
    stk::Stk::setSampleRate(44100.0);
    stk::Stk::setRawwavePath("./stk-4.6.0/rawwaves");

    TickData data;
    RtAudio dac;

    RtAudio::StreamParameters parameters;
    
    parameters.deviceId  = dac.getDefaultOutputDevice();
    parameters.nChannels = 1;

    RtAudioFormat format           = (sizeof(stk::StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned      int bufferFrames = stk::RT_BUFFER_SIZE;

    try
    {
        dac.openStream(&parameters, NULL, format, (unsigned int) stk::Stk::sampleRate(), &bufferFrames, &tick, (void *) &data);
    }
    catch (RtAudioError &error)
    {
        error.printMessage();
        return;
    }

    try
    {
        data.instrument = new stk::Plucked(CENTRAL_C);
    }
    catch (stk::StkError &)
    {
        delete data.instrument;
        return;
    }

    try
    {
        dac.startStream();
    }
    catch (RtAudioError &error)
    {
        error.printMessage();
        delete data.instrument;
        return;
    }

    while(!canExit)
    {
        data.frequency = stk::Midi2Pitch[pitch];
        if (mainRand() < (float) style && !mute)
        {
            data.instrument->noteOn(data.frequency, velocity);
        }

        while(!data.done)
        {
            stk::Stk::sleep(noteLength);
        }

        data.counter = 0;
        data.done = false;
    }

    try
    {
        dac.closeStream();
    }
    catch (RtAudioError &error)
    {
        error.printMessage();
    }

    delete data.instrument;

    return;
}

/**
 * Timer function to control stepping
 *
 * @param int val
 * @return void
 */
void timer(int val) {
    // glutPostRedisplay();
    // glutTimerFunc(16, timer, 0);
    swarm.swarm();
    playMusic();
}

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    // initGraphics();
    // glutInit(&argc, argv);
    // glfwSetKeyCallback(window, (GLFWkeyfun) special);
    // glfwGetWindowSize(window, NULL, NULL);
    // glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    // int width = w;
    // glfwSetCharCallback(window, (GLFWcharfun)special);
    // glfwSetFramebufferSizeCallback(window, reshape);
    // glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButton    // TwInit(TW_OPENGL, NULL);
    // TwWindowSize(1280, 720);GLUT);
    // glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
    // glutSpecialFunc(special);
    // TwGLUTModifiersFunc(glutGetModifiers);
    // glutMainLoop();

    // if (!glfwInit())
    // {
    //     glfwTerminate();
    //     std::exit(EXIT_FAILURE);
    // }

    // positionX = 1280 - 200;

    // glfwSetErrorCallback(errorCallback);
    // GLFWwindow* window = glfwCreateWindow(1280, 720, "Swarm Music", NULL, NULL);
    // if (!window)
    // {
    //     glfwTerminate();
    //     std::exit(EXIT_FAILURE);
    // }
    // glfwMakeContextCurrent(window);

    // glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    // glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
    // glfwSetKeyCallback(window, keyCallback);
    // glfwSetWindowSizeCallback(window, reshape);
    // defineUI();
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    // glEnable(GL_POINT_SMOOTH);
    // glfwSwapInterval(1);
    // reshape(window, 1280, 720);

    // std::thread soundThread(music);
    // while (!glfwWindowShouldClose(window))
    // {
    //     render(window);
    //     glfwSwapBuffers(window);
    //     glfwPollEvents();

    //     swarm.swarm();
    //     // playMusic();
    // }
    // soundThread.join();
    // TwTerminate();
    // glfwTerminate();

    static GLFWwindow *window;
    int width = 0, height = 0;

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Swarm Music", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 1280, 720);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // UI
    struct nk_glfw glfw = {0};
    struct nk_context* context = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
	struct nk_font_atlas* atlas;
	nk_glfw3_font_stash_begin(&glfw, &atlas);
	nk_glfw3_font_stash_end(&glfw);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		nk_glfw3_new_frame(&glfw); 
        drawUI(&glfw, context);

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

		glfwSwapBuffers(window);
	}

	nk_glfw3_shutdown(&glfw);
	glfwTerminate();
    return 0;
}
