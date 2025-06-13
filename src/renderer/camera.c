#include "camera.h"
#include "../core/input.h"
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <math.h>

#define DEFAULT_YAW -90.0f
#define DEFAULT_PITCH 0.0f
#define DEFAULT_SPEED 2.5f
#define DEFAULT_SENSITIVITY 0.1f
#define DEFAULT_ZOOM 60.0f

camera_t camera_create(vec3 pos) {
    camera_t camera;

    glm_vec3_copy(pos, camera.pos);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.world_up);

    camera.yaw = DEFAULT_YAW;
    camera.pitch = DEFAULT_PITCH;
    camera.speed = DEFAULT_SPEED;
    camera.sensitivity = DEFAULT_SENSITIVITY;
    camera.zoom = DEFAULT_ZOOM;

    camera_update_vectors(&camera);

    return camera;
}

void camera_get_view_matrix(camera_t *camera, mat4 view_matrix) {
    vec3 center;

    glm_vec3_add(camera->pos, camera->front, center);
    glm_lookat(camera->pos, center, camera->up, view_matrix);
}

void camera_update_vectors(camera_t *camera) {
    // Calc new front vector
    vec3 front;
    front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    front[1] = sin(glm_rad(camera->pitch));
    front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));

    glm_vec3_normalize(front);
    glm_vec3_copy(front, camera->front);

    // Calc right and up vectors
    glm_vec3_cross(camera->front, camera->world_up, camera->right);
    glm_vec3_normalize(camera->right);
    glm_vec3_cross(camera->right, camera->front, camera->up);
    glm_vec3_normalize(camera->up);
}

void camera_process_input(camera_t *camera, GLFWwindow *window, float delta_time) {
    float velocity = camera->speed * delta_time;

    if (input_is_key_pressed(window, GLFW_KEY_W)) {
        vec3 movement;

        glm_vec3_scale(camera->front, velocity, movement);
        glm_vec3_add(camera->pos, movement, camera->pos);
    }

    if (input_is_key_pressed(window, GLFW_KEY_S)) {
        vec3 movement;

        glm_vec3_scale(camera->front, velocity, movement);
        glm_vec3_sub(camera->pos, movement, camera->pos);
    }

    if (input_is_key_pressed(window, GLFW_KEY_A)) {
        vec3 movement;

        glm_vec3_scale(camera->right, velocity, movement);
        glm_vec3_sub(camera->pos, movement, camera->pos);
    }

    if (input_is_key_pressed(window, GLFW_KEY_D)) {
        vec3 movement;

        glm_vec3_scale(camera->right, velocity, movement);
        glm_vec3_add(camera->pos, movement, camera->pos);
    }

    if (input_is_key_pressed(window, GLFW_KEY_SPACE)) {
        vec3 movement;

        glm_vec3_scale(camera->world_up, velocity, movement);
        glm_vec3_add(camera->pos, movement, camera->pos);
    }

    if (input_is_key_pressed(window, GLFW_KEY_LEFT_SHIFT)) {
        vec3 movement;

        glm_vec3_scale(camera->world_up, velocity, movement);
        glm_vec3_sub(camera->pos, movement, camera->pos);
    }

    double dx, dy;
    input_get_mouse_delta(&dx, &dy);
    camera_process_mouse(camera, dx, dy);
}

void camera_process_mouse(camera_t *camera, double dx, double dy) {
    dx *= camera->sensitivity;
    dy *= camera->sensitivity;

    camera->yaw += dx;
    camera->pitch += dy;

    // Constrain pitch to prevent screen flipping
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    camera_update_vectors(camera);
}