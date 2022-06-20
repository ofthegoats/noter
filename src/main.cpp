#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_X11
#define EASYTAB_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <X11/Xlib.h>
#include <easytab.h>
#include <glad/glad.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // set up GLFW and OpenGL window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "drawer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Get native X11 display and window for easytab
    Display* x11Display = glfwGetX11Display();
    Window x11Window = glfwGetX11Window(window);
    if (EasyTab_Load(x11Display, x11Window) != EASYTAB_OK) {
        std::cerr << "Failed to load EasyTab" << std::endl;
    }

    // TODO set up vertices, buffers etc

    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // normalised x and y position of cursor
        double posx, posy;
        // width and height of the window used to normalise posx,posy
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        // pressure in [0, 1]
        double pressure = 0;

        XEvent xevent;
        XNextEvent(x11Display, &xevent);
        if (EasyTab_HandleEvent(&xevent) == EASYTAB_OK) {
            posx = 2 * (double)EasyTab->PosX / width - 1;
            posy = -2 * (double)EasyTab->PosY / height + 1;
            pressure = (double)EasyTab->Pressure;
            std::cout << "x = " << (float)EasyTab->PosX;
            std::cout << "  y = " << (float)EasyTab->PosY;
            std::cout << "  p = " << EasyTab->Pressure << std::endl;
        } else {
            glfwGetCursorPos(window, &posx, &posy);
            posx = 2 * posx / width - 1;
            posy = -2 * posy / height + 1;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                pressure = 0.5;
        }

#ifndef NDEBUG
        std::cout << "x = " << posx << " y = " << posy << " p = " << pressure << std::endl;
#endif

        // TODO render here

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    EasyTab_Unload(x11Display);
    return 0;
}
