#ifndef __MOUSE__
#define __MOUSE__

  #include "orbit-camera.h"

  struct {
    uint8_t down;
    float x, y;
  } mouse;

  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    mouse.down = action == GLFW_PRESS;
  }

  void mouse_move_callback(GLFWwindow* window, double x, double y) {

    if (mouse.down) {
      int w, h;
      glfwGetWindowSize(window, &w, &h);
      orbit_camera_rotate(0, 0, (mouse.x - x) / w, (mouse.y - y) / h);
    }

    mouse.x = x;
    mouse.y = y;
  }
#endif
