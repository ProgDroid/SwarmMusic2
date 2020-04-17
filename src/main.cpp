/**
 * Main class
 *
 * @package Swarm Music
 * @author Fernando Ferreira
 */

#include <Agent.h>
#include <algorithm>
#include <AntTweakBar.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <random>
#include <stdio.h>
#include <stk/Plucked.h>
#include <stk/RtAudio.h>
#include <stk/Skini.h>
#include <Swarm.h>
#include <thread>
#include <Triplet.h>

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
float theta                = 0.0;
bool  canExit              = false;
int   style                = JAZZ;
int   GUIpitch             = 72;
int   pitchMax             = 96;
int   GUIscale             = 0;
bool  mute                 = false;
Swarm swarm;
TwBar *upperBar;
TwBar *lowerBar;
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

void TW_CALL addAttr(void *)
{
    swarm.addAttractor(GUIpitch, -1);
}

void TW_CALL muteSound(void *)
{
    mute = !mute;
}

void TW_CALL makeScale(void *)
{
    swarm.resetAttractors();

    int root      = GUIpitch;
    int scaleType = GUIscale;

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

// * Radius callbacks

void TW_CALL setRepulsion(const void *value, void *clientData)
{
    swarm.setRepulsionRadius(*(const float *)value);
}

void TW_CALL setOrientation(const void *value, void *clientData)
{
    swarm.setOrientationRadius(*(const float *)value);
}

void TW_CALL setAttraction(const void *value, void *clientData)
{
    swarm.setAttractionRadius(*(const float *)value);
}

void TW_CALL getRepulsion(void *value, void *clientData)
{
    *(float *)value = swarm.getRepulsionRadius();
}

void TW_CALL getOrientation(void *value, void *clientData)
{
    *(float *)value = swarm.getOrientationRadius();
}

void TW_CALL getAttraction(void *value, void *clientData)
{
    *(float *)value = swarm.getAttractionRadius();
}

// * Blind angle, speed and maximum force callbacks

void TW_CALL setBlindAngle(const void *value, void *clientData)
{
    swarm.setBlindAngle(*(const float *)value);
}

void TW_CALL setSpeed(const void *value, void *clientData)
{
    swarm.setSpeed(*(const float *)value);
}

void TW_CALL setMaxForce(const void *value, void *clientData)
{
    swarm.setMaxForce(*(const float *)value);
}

void TW_CALL getBlindAngle(void *value, void *clientData)
{
    *(float *)value = swarm.getBlindAngle();
}

void TW_CALL getSpeed(void *value, void *clientData)
{
    *(float *)value = swarm.getSpeed();
}

void TW_CALL getMaxForce(void *value, void *clientData)
{
    *(float *)value = swarm.getMaxForce();
}

/**
 * Define the UI elements
 * todo refactor to own class?
 *
 * @return void
 */
void defineUI()
{
    // global ui properties
    TwDefine("GLOBAL help='The Free Sound of Self-Organisation' fontsize=1 fontresizable=false");

    // left sidebar
    upperBar = TwNewBar("Swarm Properties");
    TwDefine("'Swarm Properties' position='0 0' size='200 120' valueswidth=35 color='193 131 159' alpha=255 movable=false resizable=false");
    // control radiuses
    TwAddVarCB(upperBar, "Repulsion Radius", TW_TYPE_FLOAT, setRepulsion, getRepulsion, &swarm, "min=10 max=40 step=1 keyIncr=r keyDecr=R help='Set repulsion radius.'");
    TwAddVarCB(upperBar, "Orientation Radius", TW_TYPE_FLOAT, setOrientation, getOrientation, &swarm, "min=41 max=70 step=1 keyIncr=o keyDecr=O help='Set orientation radius.'");
    TwAddVarCB(upperBar, "Attraction Radius", TW_TYPE_FLOAT, setAttraction, getAttraction, &swarm, "min=71 max=150 step=1 keyIncr=a keyDecr=A help='Set repulsion radius.'");

    // control blind angle, speed and maximum force
    TwAddVarCB(upperBar, "Blind angle", TW_TYPE_FLOAT, setBlindAngle, getBlindAngle, &swarm, "min=0 max=45 step=1 keyIncr=b keyDecr=B help='Set blind angle.'");
    TwAddVarCB(upperBar, "Speed", TW_TYPE_FLOAT, setSpeed, getSpeed, &swarm, "min=0.5 max=3.5 step=0.1 keyIncr=s keyDecr=S help='Set speed.'");
    TwAddVarCB(upperBar, "Maximum Force", TW_TYPE_FLOAT, setMaxForce, getMaxForce, &swarm,"min=0.1 max=2.0 step=0.1 keyIncr=f keyDecr=F help='Set maximum force.'");

    // control theta manually/check value
    TwAddVarRW(upperBar, "View Angle", TW_TYPE_FLOAT, &theta,"min=0.0 max=360.0 help='Set view angle.'");

    // right sidebar
    lowerBar = TwNewBar("Musical Properties");
    TwDefine("'Musical Properties' position='0 150' size='200 120' valueswidth=35 color='193 131 159' alpha=255 movable=false resizable=false");
    // control chance of note being played
    TwEnumVal styleMode[] =
    {
        {DOOM, "DOOM"},
        {BLUES, "Blues"},
        {JAZZ, "Jazz"},
        {METAL, "Metal"},
        {FIREANDFLAMES, "Insane"}
    };
    TwType modeType = TwDefineEnum("Style", styleMode, 5);
    TwAddVarRW(lowerBar, "Style", modeType, &style, "keyIncr=n keyDecr=N help='From a gloomy to non-stop.'");

    // control whether to get random coordinates from agents or average of all
    TwEnumVal swarmMode[] =
    {
        {RANDOM, "Random"},
        {AVERAGE, "Average"},
    };
    TwType swarmType = TwDefineEnum("Mapping", swarmMode, 2);
    TwAddVarRW(lowerBar, "Mapping", swarmType, &swarm.swarmMode, "keyIncr=m keyDecr=M help='Pick random coordinates from swarm agents or average of all.'");

    TwEnumVal pitchMode[] =
    {
        {72, "C4"},  {73, "C#4"}, {74, "D4"},  {75, "D#4"},
        {76, "E4"},  {77, "F4"},  {78, "F#4"}, {79, "G4"},
        {80, "G#4"}, {81, "A4"},  {82, "A#4"}, {83, "B4"},
        {84, "C5"},  {85, "C#5"}, {86, "D5"},  {87, "D#5"},
        {88, "E5"},  {89, "F5"},  {90, "F#5"}, {91, "G5"},
        {92, "G#5"}, {93, "A5"},  {94, "A#5"}, {95, "B5"},
        {96, "C6"}
    };
    TwType pitchType = TwDefineEnum("Pitches", pitchMode, 25);

    // pick pitch for new attractor
    TwAddVarRW(lowerBar, "Pitch", pitchType, &GUIpitch, "label='Pitch for attractors or scale' keyIncr='p' keyDecr='P' help='The pitch for attractors to be added.'");

    // add attractor
    TwAddButton(lowerBar, "Add Attractor", addAttr, nullptr, "label='Add an attractor' help='Add attractor with pitch from above.'");

    // pick scale type
    TwEnumVal scaleMode[] =
    {
        {MAJOR, "Major"},
        {MINOR, "Minor"},
        {PENTATONIC, "Pentatonic Minor"},
        {BLUES, "Blues"},
    };
    TwType scalesType = TwDefineEnum("Scale Type", scaleMode, 4);
    TwAddVarRW(lowerBar, "Scale Type", scalesType, &GUIscale, "keyIncr=t \
             keyDecr=T help='Pick the type of scale to be added with attractors.'");
    // button to make scale based on pitch
    TwAddButton(lowerBar, "Add Scale", makeScale, nullptr, "label='Add a scale' \
              help='Add a scale with the pitch above as root.'");
    // button to mute/unmute
    TwAddButton(lowerBar, "Mute On/Off", muteSound, nullptr, "label='Mute On/Off' \
              help='Mute sound output.'");
}

void drawWireCube()
{
    // glColor4f(0.0, 0.0, 0.0, 1.0);
    // glPushMatrix();
    // glutWireCube(800.0);
    // glPopMatrix();
}

void render(GLFWwindow* window)
{
    glLoadIdentity();
    gluLookAt(1250 * sin(theta * (PI / 180)), 500.0, 1250 * cos(theta * (PI / 180)),
              0.0, -100.0, 0.0,
              0.0, 1.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT);

    swarm.drawAgents();
    // drawWireCube();
    swarm.drawAttractors();

    // draw UI
    TwDraw();
}

void reshape(GLFWwindow* window, int width, int height)
{
    printf("welele");
    glClearColor(OLIVEBLACK, OLIVEBLACK, OLIVEBLACK, 1.0);
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    TwWindowSize(width, height);
    positionX = width - 200;
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

    if (key == GLUT_KEY_PAGE_UP && action == GLFW_PRESS)
    {
        swarm.resetAll();
    }

    if (key == GLUT_KEY_PAGE_DOWN && action == GLFW_PRESS)
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

void initGraphics()
{
    if (!glfwInit())
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    };
    // glutInit(&argc, argv);
    // glutInitWindowSize(1280, 720);
    // glutInitWindowPosition(100, 100);
    positionX = 1280 - 200;
    // glutInitDisplayMode(GLUT_DOUBLE);
    // glutCreateWindow("Swarm Music");
    // glutDisplayFunc(display);
    // glutSpecialFunc(special);
    // glutReshapeFunc(reshape);
    // glutIdleFunc(idle);
    // glutTimerFunc(0, timer, 0);
}

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
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
    // glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    // glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
    // glutSpecialFunc(special);
    // TwGLUTModifiersFunc(glutGetModifiers);
    // glutMainLoop();

    if (!glfwInit())
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    positionX = 1280 - 200;

    glfwSetErrorCallback(errorCallback);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Swarm Music", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    TwInit(TW_OPENGL, NULL);
    TwWindowSize(1280, 720);

    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, reshape);
    defineUI();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_POINT_SMOOTH);
    std::thread soundThread(music);
    while (!glfwWindowShouldClose(window))
    {
        render(window);
        glfwSwapBuffers(window);
        glfwPollEvents();

        swarm.swarm();
        playMusic();
    }
    soundThread.join();
    TwTerminate();
    glfwTerminate();

    return 0;
}
