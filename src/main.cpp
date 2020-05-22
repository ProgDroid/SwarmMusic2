/**
 * Main class
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <math.h>
#include <random>
#include <stdio.h>
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
const int FIREANDFLAMES    = 100;
const int METAL            = 80;
const int JAZZ             = 60;
const int BLUES            = 40;
const int DOOM             = 20;
const int MAJOR            = 0;
const int MINOR            = 1;
const int PENTATONIC       = 2;
const int BLUES_SCALE      = 3;
const int MAJOR_SIZE       = 7;
const int MINOR_SIZE       = 7;
const int PENTATONIC_SIZE  = 5;
const int BLUES_SIZE       = 6;
const int RANDOM           = 0;
const int AVERAGE          = 1;
const int LENGTH_FACTOR    = 250;

static float repulsionRadius   = 20.0;
static float orientationRadius = 50.0;
static float attractionRadius  = 110.0;
static float blindAngle        = 10;
static float speed             = 1.5;
static float maxForce          = 0.7;

float theta                = 0.0;
bool  canExit              = false;
int   style                = JAZZ;
int   GUIpitch             = 72;
int   pitchMax             = 96;
int   GUIscale             = 0;
bool  mute                 = false;

Swarm swarm;
long pitch;
float velocity;
float noteLength;
int positionX;

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

// void TW_CALL addAttr(void *)
// {
//     swarm.addAttractor(GUIpitch, -1);
// }

// void TW_CALL muteSound(void *)
// {
//     mute = !mute;
// }

// void TW_CALL makeScale(void *)
// {
//     swarm.resetAttractors();

//     int root      = GUIpitch;
//     int scaleType = GUIscale;

//     // normalise root to make 2 octave scale
//     if (root >= 84)
//     {
//         root -= 12;
//     }

//     std::vector<int> intervals;

//     // todo scales as objects?
//     if (scaleType == MAJOR)
//     {
//         intervals.reserve(MAJOR_SIZE);
//         intervals.push_back(2);
//         intervals.push_back(2);
//         intervals.push_back(1);
//         intervals.push_back(2);
//         intervals.push_back(2);
//         intervals.push_back(2);
//         intervals.push_back(1);
//     }
//     else if (scaleType == MINOR)
//     {
//         intervals.reserve(MINOR_SIZE);
//         intervals.push_back(2);
//         intervals.push_back(1);
//         intervals.push_back(2);
//         intervals.push_back(2);
//         intervals.push_back(1);
//         intervals.push_back(2);
//         intervals.push_back(2);
//     }
//     else if (scaleType == PENTATONIC)
//     {
//         intervals.reserve(PENTATONIC_SIZE);
//         intervals.push_back(3);
//         intervals.push_back(2);
//         intervals.push_back(2);
//         intervals.push_back(3);
//         intervals.push_back(2);
//     }
//     else if (scaleType == BLUES)
//     {
//         intervals.reserve(BLUES_SIZE);
//         intervals.push_back(3);
//         intervals.push_back(2);
//         intervals.push_back(1);
//         intervals.push_back(1);
//         intervals.push_back(3);
//         intervals.push_back(2);
//     }

//     // 2 iterations over the intervals to make 2 octaves scale
//     for (int i = 0, size = intervals.size(); i < size * 2; ++i)
//     {
//         int index = i % size;
//         swarm.addAttractor(root + intervals[index], index);
//     }
// }

void slider(nk_context *context, const char *label, float min, float max, float *value, float step) {
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
    if (nk_begin(context,
                    "Swarm Properties",
                    nk_rect(0, 0, 255, 155),
                    NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR
            )
        ) {
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
    }
    nk_end(context);
}

//     // upper sidebar (swarm properties)
//     upperBar = TwNewBar("Swarm Properties");
//     TwDefine("'Swarm Properties' position='0 0' size='200 120' valueswidth=35 color='193 131 159' alpha=255 movable=false resizable=false");

//     // control theta manually/check value
//     TwAddVarRW(upperBar, "View Angle", TW_TYPE_FLOAT, &theta,"min=0.0 max=360.0 help='Set view angle.'");

//     // lower sidebar (musical properties)
//     lowerBar = TwNewBar("Musical Properties");
//     TwDefine("'Musical Properties' position='0 150' size='200 120' valueswidth=35 color='193 131 159' alpha=255 movable=false resizable=false");
//     // control chance of note being played
//     TwEnumVal styleMode[] =
//     {
//         {DOOM, "DOOM"},
//         {BLUES, "Blues"},
//         {JAZZ, "Jazz"},
//         {METAL, "Metal"},
//         {FIREANDFLAMES, "Insane"}
//     };
//     TwType modeType = TwDefineEnum("Style", styleMode, 5);
//     TwAddVarRW(lowerBar, "Style", modeType, &style, "keyIncr=n keyDecr=N help='From a gloomy to non-stop.'");

//     // control whether to get random coordinates from agents or average of all
//     TwEnumVal swarmMode[] =
//     {
//         {RANDOM, "Random"},
//         {AVERAGE, "Average"},
//     };
//     TwType swarmType = TwDefineEnum("Mapping", swarmMode, 2);
//     TwAddVarCB(lowerBar, "Mapping", swarmType, setSwarmMode, getSwarmMode, &swarm, "keyIncr=m keyDecr=M help='Pick random coordinates from swarm agents or average of all.'");

//     TwEnumVal pitchMode[] =
//     {
//         {72, "C4"},  {73, "C#4"}, {74, "D4"},  {75, "D#4"},
//         {76, "E4"},  {77, "F4"},  {78, "F#4"}, {79, "G4"},
//         {80, "G#4"}, {81, "A4"},  {82, "A#4"}, {83, "B4"},
//         {84, "C5"},  {85, "C#5"}, {86, "D5"},  {87, "D#5"},
//         {88, "E5"},  {89, "F5"},  {90, "F#5"}, {91, "G5"},
//         {92, "G#5"}, {93, "A5"},  {94, "A#5"}, {95, "B5"},
//         {96, "C6"}
//     };
//     TwType pitchType = TwDefineEnum("Pitches", pitchMode, 25);

//     // pick pitch for new attractor
//     TwAddVarRW(lowerBar, "Pitch", pitchType, &GUIpitch, "label='Pitch for attractors or scale' keyIncr='p' keyDecr='P' help='The pitch for attractors to be added.'");

//     // add attractor
//     TwAddButton(lowerBar, "Add Attractor", addAttr, nullptr, "label='Add an attractor' help='Add attractor with pitch from above.'");

//     // pick scale type
//     TwEnumVal scaleMode[] =
//     {
//         {MAJOR, "Major"},
//         {MINOR, "Minor"},
//         {PENTATONIC, "Pentatonic Minor"},
//         {BLUES, "Blues"},
//     };
//     TwType scalesType = TwDefineEnum("Scale Type", scaleMode, 4);
//     TwAddVarRW(lowerBar, "Scale Type", scalesType, &GUIscale, "keyIncr=t \
//              keyDecr=T help='Pick the type of scale to be added with attractors.'");
//     // button to make scale based on pitch
//     TwAddButton(lowerBar, "Add Scale", makeScale, nullptr, "label='Add a scale' \
//               help='Add a scale with the pitch above as root.'");
//     // button to mute/unmute
//     TwAddButton(lowerBar, "Mute On/Off", muteSound, nullptr, "label='Mute On/Off' \
//               help='Mute sound output.'");
// }

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
    else if (style == FIREANDFLAMES)
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

// TODO style
    // context->style.slider.normal = nk_style_item_color(nk_rgba(0, 0, 255, 255));

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
