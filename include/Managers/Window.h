#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Managers {

class Window
{
private:
    GLFWwindow* window;

    float capValue;

public:
    Window();
    ~Window();

    static void errorCallback(const int error, const char* description);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void windowCloseCallback(GLFWwindow* window);
    static void windowFocusCallback(GLFWwindow*, int focused);
    static void windowIconifyCallback(GLFWwindow* window, int iconified);

    GLFWwindow* getWindow() const;

    float getCapValue() const;

    void getWindowDimensions(int* width, int* height) const;
    void pollEvents() const;
    void setCapValue(float value);
    void swapBuffers();

    bool shouldClose() const;
};

}

#endif
