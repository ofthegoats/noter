#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <queue>
#include <utility>

#include "noter.hpp"
#include "signals.hpp"


std::queue<std::queue<std::pair<double, double>>> Noter::drawQueue;
std::queue<Signals> Noter::signalQueue;

void Noter::framebufferSizeCallback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

void Noter::keyCallback(GLFWwindow *window, int key, int, int action, int mods) {
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

void Noter::mouseButtonCallback(GLFWwindow*, int button, int action, int) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    userIsDrawing = true;
    signalQueue.push(StrokeStarted);
    drawQueue.push(std::queue<std::pair<double, double>>());
  } else if (action == GLFW_RELEASE) {
    userIsDrawing = false;
    signalQueue.back() = StrokeFinished;
  }
}

void Noter::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
  if (userIsDrawing) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    drawQueue.front().push({2 * (xpos / width) - 1, 1 - 2 * (ypos / height)});
  }
}
