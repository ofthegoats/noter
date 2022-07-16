#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <queue>
#include <utility>
#include <assert.h>

#include "callbacks.hpp"
#include "enums.hpp"

// queue of strokes containing vertices to draw to
std::queue<std::queue<std::pair<double, double>>> drawQueue;
std::queue<Signals> signalQueue;

int main() {
  // glfw setup
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  // window-specific hints
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  // make window
  GLFWwindow *window = glfwCreateWindow(1200, 800, "noter", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window\n";
    return 1;
  }
  glfwMakeContextCurrent(window);
  // load glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return 1;
  }
  glViewport(0, 0, 1200, 800);

  glfwSetWindowPos(window, 520, 200);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // enable antialiasing - should be on by default on newer hardware
  glEnable(GL_MULTISAMPLE_BIT);

  // background for front and back buffer
  glClearColor((float)29/255, (float)32/255, (float)33/255, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);
  glClear(GL_COLOR_BUFFER_BIT);


  // for now, only one drawing colour and width is available
  glColor3f((float)204 / 255, (float)36 / 255, (float)29 / 255);
  glLineWidth(3);

  while (!glfwWindowShouldClose(window)) {

    // drawing in immediate mode
    glBegin(GL_LINE_STRIP);
    // draw as long as we can
    bool drew = false;
    while (drawQueue.size() >= 1 && drawQueue.front().size() > 1) {
      glVertex2d(drawQueue.front().front().first, drawQueue.front().front().second);
      drawQueue.front().pop();
      drew = true;
    }
    if (drew && drawQueue.front().size() >= 1) {
      glVertex2d(drawQueue.front().front().first, drawQueue.front().front().second);
    }
    glEnd();

    // if we're done, pop the stroke
    while (signalQueue.size() > 0) {
      switch (signalQueue.front()) {
      case StrokeStarted: {
	// nothing to do but wait for the user to finish, need to exit loop
	goto exit_signal_loop;
      }
      case StrokeFinished: {
	drawQueue.pop();
	signalQueue.pop();
	break;
      }
      case UndoRequested: {
	std::cout << "undo here" << std::endl; // TODO
	signalQueue.pop();
	break;
      }
      case RedoRequested: {
	break;
      }
      default: throw "wrong signal received";
      }
    }
  exit_signal_loop:

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

