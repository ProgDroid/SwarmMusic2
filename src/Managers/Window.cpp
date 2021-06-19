#include <Managers/Window.h>

#include <iostream>

namespace Managers {

Window::Window() {
    int width, height;
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        glfwTerminate();
        throw std::runtime_error("Could not init GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "Swarm Music", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetWindowSizeLimits(window, 720, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        glfwTerminate();
        throw std::runtime_error("Failed to get primary monitor");
    }

    const GLFWvidmode *vidMode = glfwGetVideoMode(monitor);
    if (!vidMode) {
        glfwTerminate();
        throw std::runtime_error("Failed to get video mode from primary monitor");
    }

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    glfwSetWindowPos(window, monitorX + (vidMode->width - width) / 2, monitorY + (vidMode->height - height) / 2);

    capValue = 1.0f / vidMode->refreshRate; // ! save vidMode as private variable then query this from elsewhere where I'll actually need it?

    glfwSwapInterval(0);
}

Window::~Window() {
    glfwTerminate();
}

void Window::errorCallback(const int error, const char* description) {
    std::cerr << "Error " << error << ": " << description << std::endl;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::pollEvents() const {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::windowCloseCallback(GLFWwindow* window) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Window::windowFocusCallback(GLFWwindow*, int focused) {
    if (focused) {
        // reduce volume?
        return;
    }
}

void Window::windowIconifyCallback(GLFWwindow* window, int iconified) {
    if (iconified) {
        // stop simulation?
        // mute?
        // ? disable systems by having a separate array for enabled and disabled ones?
        return;
    }
}

GLFWwindow* Window::getWindow() const {
    return window;
}

void Window::getWindowDimensions(int* width, int* height) const {
    glfwGetWindowSize(window, width, height);
}

float Window::getCapValue() const {
    return capValue;
}

void Window::setCapValue(float value) {
    capValue = value;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

}
