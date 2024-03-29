#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <utility>
#include <fstream>
#include <string>

#include "signals.hpp"
#include "noter.hpp"

// Initialise GLFW, glad, shaders, OpenGL buffers
Noter::Noter() {
  if (!glfwInit()) {
    std::cerr << "Noter::Noter  > glfwInit() failed" << std::endl;
    std::exit(1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  window = glfwCreateWindow(1200, 800, "noter", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window\n";
    std::exit(1);
  }
  glfwMakeContextCurrent(window);
  // load glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::exit(1);
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

  // Compile and link shaders
  //?TODO read shader files AT COMPILE TIME (but they are compiled and linked at runtime)
  int success;
  char infoLog[512];
  const char *vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;

void main() {
  gl_Position = vec4(aPos, 0.0, 1.0);
}
)";
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Noter::Noter > Failed to build vertex shader > " << infoLog << std::endl;
    std::exit(1);
  }
  const char *fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

void main() {
  FragColor = vec4(0.8, 0.141176470588, 0.113725490196, 1.0f);
}
)";
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::cerr << "Noter::Noter > Failed to build fragment shader > " << infoLog << std::endl;
    std::exit(1);
  }
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    std::cerr << "Noter::Noter > Failed to link shader > " << infoLog << std::endl;
    std::exit(1);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // NOTE because the object to draw changes often, initialization code is in Noter::run
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

Noter::~Noter() {
  glfwTerminate();
}

void Noter::run() {
  // for now, only one line width is possible
  glLineWidth(3);
  // for now, the colour of the line is hardcoded in the fragment shader
  // later it would be better to use layouts and interact with an imgui window
  while (!glfwWindowShouldClose(window)) {
    // draw as long as we can
    bool drew = false;
    std::vector<double> vertices;
    while (drawQueue.size() >= 1 && drawQueue.front().size() > 1) {
      vertices.push_back(drawQueue.front().front().first);
      vertices.push_back(drawQueue.front().front().second);
      drawQueue.front().pop();
      drew = true;
    }
    if (drew && drawQueue.front().size() > 0) {
      vertices.push_back(drawQueue.front().front().first);
      vertices.push_back(drawQueue.front().front().second);
    }
    if (drew) {
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(double), vertices.data(), GL_STREAM_DRAW);
      glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 0, (void*)0);
      glEnableVertexAttribArray(0);
      glUseProgram(shaderProgram);
      glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 2);
    }

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
}
