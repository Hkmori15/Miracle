#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

/**
   * Init GLFW and create a window with OpenGL context
   * @param width Window width in pixels
   * @param height Window height in pixels
   * @param title Window title string
   * @return GLFWwindow pointer on success and NULL on failure
**/

GLFWwindow* window_init(int width, int height, const char* title);

/**
   * Check if window should close
   * @param window GLFW window pointer
   * @return 1 if window should close and 0 otherwise
**/

int window_should_close(GLFWwindow* window);

/**
   * Update window - swap buffers and poll events
   * @param window GLFW window pointer
**/

void window_update(GLFWwindow* window);

/**
   * Terminate GLFW and cleanup resources
**/

void window_terminate(void);

#endif // WINDOW_H
