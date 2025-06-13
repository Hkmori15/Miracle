#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "model.h"
#include <cglm/cglm.h>

/**
   * Init the renderer
**/

void renderer_init(void);

/**
   * Begin a new frame
   * @param camera Camera for view matrix
   * @param projection Projection matrix
**/

void renderer_begin_frame(camera_t* camera, mat4 projection);

/**
   * Submit a model for rendering
   * @param model Model to render
   * @param transform Model transformation matrix
   * @param shader Shader program ID
**/

void renderer_submit(model_t model, mat4 transform, unsigned int shader);

/**
   * End the current frame
**/

void renderer_end_frame(void);

/**
   * Set clear color
   * @param r - Red component: 0.0-1.0
   * @param g - Green component: 0.0-1.0
   * @param b - Blue component: 0.0-1.0
   * @param a - Alpha component: 0.0-1.0
**/

void renderer_set_clear_color(float r, float g, float b, float a);

#endif // RENDERER_H
