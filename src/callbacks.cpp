#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <queue>
#include <utility>

#include "callbacks.hpp"
#include "enums.hpp"

void framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}


extern std::queue<std::queue<std::pair<double, double>>> drawQueue;
extern std::queue<Signals> signalQueue;

void keyCallback(GLFWwindow *window, int key, int, int action, int mods) {
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (key == GLFW_KEY_Z && mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
    // if in the middle of a stroke, finish it
    bool midStroke = false;
    if (signalQueue.size() > 0 && signalQueue.back() == StrokeStarted) {
      signalQueue.back() = StrokeFinished;
      midStroke = true;
    }
    signalQueue.push(UndoRequested);
    if (midStroke) {
      signalQueue.push(StrokeStarted);
      drawQueue.push(std::queue<std::pair<double, double>>());
    }
  }
}

bool drawing = false;

void mouseButtonCallback(GLFWwindow*, int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    drawing = true;
    signalQueue.push(StrokeStarted);
    drawQueue.push(std::queue<std::pair<double, double>>());
  } else if (action == GLFW_RELEASE) {
    drawing = false;
    signalQueue.back() = StrokeFinished;
  }
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
  if (drawing) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    drawQueue.front().push({2 * (xpos / width) - 1, 1 - 2 * (ypos / height)});
  }
}
