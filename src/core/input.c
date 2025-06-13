#include "input.h"
#include <GLFW/glfw3.h>

static double last_mouse_x = 0.0;
static double last_mouse_y = 0.0;
static double mouse_delta_x = 0.0;
static double mouse_delta_y = 0.0;
static int first_mouse = 1;

void input_init(GLFWwindow *window) {
    // Get init mouse pos to prevent large jump on first frame
    glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);

    first_mouse = 1;
    mouse_delta_x = 0.0;
    mouse_delta_y = 0.0;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

int input_is_key_pressed(GLFWwindow *window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void input_get_mouse_pos(GLFWwindow *window, double *x, double *y) {
    glfwGetCursorPos(window, x, y);
}

void input_get_mouse_delta(double *dx, double *dy) {
    *dx = mouse_delta_x;
    *dy = mouse_delta_y;
}

void input_update(GLFWwindow *window) {
    double current_x, current_y;
    glfwGetCursorPos(window, &current_x, &current_y);

    if (first_mouse) {
        last_mouse_x = current_x;
        last_mouse_y = current_y;
        first_mouse = 0;
        mouse_delta_x = 0.0;
        mouse_delta_y = 0.0;
    } else {
        mouse_delta_x = current_x - last_mouse_x;
        // Reversed since y-coordinates go from bottom to top 
        mouse_delta_y = last_mouse_y - current_y;
    }

    last_mouse_x = current_x;
    last_mouse_y = current_y;
}