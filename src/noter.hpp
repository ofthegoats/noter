#include <GLFW/glfw3.h>
#include <queue>
#include <utility>

#include "signals.hpp"

class Noter {
private:
  static std::queue<std::queue<std::pair<double, double>>> drawQueue;
  static std::queue<Signals> signalQueue;
  inline static bool userIsDrawing = false;
  GLFWwindow *window;

  // CALLBACKS
  static void framebufferSizeCallback(GLFWwindow *, int width, int height);
  static void keyCallback(GLFWwindow *, int key, int scancode, int action, int mods);
  static void mouseButtonCallback(GLFWwindow *, int button, int action, int mods);
  static void cursorPositionCallback(GLFWwindow *, double xpos, double ypos);

public:
  // Initialise buffers used in mainloop
  Noter();
  // Clean up GLFW
  ~Noter();
  // main loop
  void run();
};
