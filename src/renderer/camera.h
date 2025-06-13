#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

typedef struct {
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;

    float yaw;
    float pitch;

    float speed;
    float sensitivity;
    float zoom;
} camera_t;

/**
    * Create a camera with default settings
    * @param pos init position
    * @return Initialized camera
**/

camera_t camera_create(vec3 pos);

/** 
    * Get the view matrix for the camera
    * @param camera Camera pointer
    * @param view_matrix Output view matrix
**/

void camera_get_view_matrix(camera_t* camera, mat4 view_matrix);

/**
    * Update camera vectors based on yaw and pitch
    * @param camera Camera pointer
**/

void camera_update_vectors(camera_t* camera);

/**
    * Process keyboard input for camera movement
    * @param camera Camera pointer
    * @param window GLFW window pointer
    * @param delta_time Time since last frame
**/

void camera_process_input(camera_t* camera, GLFWwindow* window, float delta_time);

/**
    * Process mouse movement for camera rotation
    * @param camera Camera pointer
    * @param dx Mouse x delta
    * @param dy Mouse y delta
**/

void camera_process_mouse(camera_t* camera, double dx, double dy);

#endif // CAMERA_H
