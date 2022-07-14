#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <queue>
#include <utility>

#include "callbacks.hpp"

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int, int action, int) {
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

extern std::queue<std::queue<std::pair<double, double>>> drawQueue;
extern std::queue<bool> strokeFinishQueue;
bool drawing = false;

void mouseButtonCallback(GLFWwindow*, int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    drawing = true;
    strokeFinishQueue.push(false);
    drawQueue.push(std::queue<std::pair<double, double>>());
  } else if (action == GLFW_RELEASE) {
    drawing = false;
    strokeFinishQueue.back() = true;
  }
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
  if (drawing) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    drawQueue.front().push({2 * (xpos / width) - 1, 1 - 2 * (ypos / height)});
  }
}
