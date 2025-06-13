#include "renderer.h"
#include "renderer/camera.h"
#include "renderer/model.h"
#include "shader.h"
#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <cglm/mat4.h>
#include <GL/gl.h>
#include <GL/glext.h>

static mat4 current_view;
static mat4 current_projection;

void renderer_init(void) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Default clear color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    printf("Renderer initialized\n");
}

void renderer_begin_frame(camera_t *camera, mat4 projection) {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera_get_view_matrix(camera, current_view);
    glm_mat4_copy(projection, current_projection);
}

void renderer_submit(model_t model, mat4 transform, unsigned int shader) {
    shader_use(shader);
    shader_set_mat4(shader, "model", transform);
    shader_set_mat4(shader, "view", current_view);
    shader_set_mat4(shader, "projection", current_projection);

    model_draw(model, shader);
}

void renderer_end_frame(void) {
    // We not doing anyhere for now
    // Could be used for post-processing etc..
    // UI rendering?
}

void renderer_set_clear_color(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}