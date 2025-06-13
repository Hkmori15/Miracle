#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* desc) {
    fprintf(stderr, "GLFW error %d: %s\n", error, desc);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;

    glViewport(0, 0, width, height);
}

GLFWwindow* window_init(int width, int height, const char* title) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");

        return NULL;
    }

    // Configure
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();

        return NULL;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Vsync
    glfwSwapInterval(1);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return window;
}

int window_should_close(GLFWwindow *window) {
    return glfwWindowShouldClose(window);
}

void window_update(GLFWwindow *window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void window_terminate(void) {
    glfwTerminate();
}