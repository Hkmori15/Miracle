#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

/**
   * Init input system with window context
   * @param window GLFW window pointer
**/

void input_init(GLFWwindow* window);

/**
   * Check if a key is currently pressed
   * @param window GLFW window pointer
   * @param key GLFW key code: e.g GLFW_KEY_W
   * @return 1 if key is pressed and 0 otherwise
**/

int input_is_key_pressed(GLFWwindow* window, int key);

/**
   * Get current mouse pos
   * @param window GLFW window pointer
   * @param x Pointer to store x coordinate
   * @param y Pointer to store y coordinate
**/

void input_get_mouse_pos(GLFWwindow* window, double* x, double* y);

/**
   * Get mouse movement delta since last call
   * @param dx Pointer to store x delta
   * @param dy Pointer to store y delta
**/

void input_get_mouse_delta(double* dx, double* dy);

/**
   * Update input state - call once per frame
   * @param window GLFW window pointer
**/

void input_update(GLFWwindow* window);

#endif // INPUT_H
