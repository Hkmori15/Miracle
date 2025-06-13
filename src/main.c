#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "core/window.h"
#include "core/input.h"
#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "renderer/camera.h"
#include "renderer/model.h"
#include "physics/physics.h"
#include "audio/audio.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "Miracle Engine"

static double last_frame = 0.0;
static double delta_time = 0.0;

// Engine state
static GLFWwindow* window = NULL;
static camera_t camera;
static unsigned int shader_program = 0;
static unsigned int texture_id = 0;
static model_t girl_model;
static model_t cube_model;
static physics_world_t physics_world;
static int current_model = 0; // 0 = cube, 1 = girl model
static btRigidBody* physics_cube = NULL;
static unsigned int kaleidoscope = 0;
static vec3 light_pos = {2.0f, 4.0f, 3.0f};
static vec3 light_color = {1.0f, 1.0f, 1.0f};

static int init_engine(void);
static void update_engine(void);
static void render_engine(void);
static void cleanup_engine(void);
static void process_input(void);

int main(void) {
    printf("Starting Miracle Engine...\n");

    if (init_engine() != 0) {
        fprintf(stderr, "Failed to initialize engine\n");
        cleanup_engine();
        return -1;
    }

    printf("Engine initialized successfully\n");
    printf("Controls:\n");
    printf("  M - Toggle between models (Cube/Girl)\n");
    printf("  1 - Play audio track Romchika\n");
    printf("  F1 - Toggle wireframe\n");
    printf("  ESC - Exit\n");

    while (!window_should_close(window)) {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        update_engine();
        render_engine();

        // Swap buffers and poll events
        window_update(window);
    }

    cleanup_engine();
    printf("Engine shutdown complete\n");
    return 0;
}

static int init_engine(void) {
    window = window_init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    if (!window) {
        return -1;
    }

    input_init(window);
    renderer_init();

    if (audio_init() != 0) {
        printf("Warning: Failed to initialize audio system\n");
    } else {
        printf("Loading audio files...\n");
        kaleidoscope = audio_load_sound("assets/audio/kaleidoscope.mp3");
        printf("Audio loading complete\n");
    }

    physics_world = physics_world_create();

    // Create physics cube
    vec3 cube_pos = {0.0f, 5.0f, 0.0f}; // Start above ground
    vec3 cube_size = {1.0f, 1.0f, 1.0f};
    physics_cube = physics_add_box(&physics_world, cube_pos, cube_size, 1.0f); // 1kg mass

    vec3 ground_pos = {0.0f, -1.0f, 0.0f};
    vec3 ground_size = {10.0f, 0.1f, 10.0f};
    physics_add_box(&physics_world, ground_pos, ground_size, 0.0f); // Static (mass = 0)

    // Create camera (move further back to see the whole cube)
    vec3 camera_pos = {0.0f, 0.0f, 5.0f};
    camera = camera_create(camera_pos);

    shader_program = shader_create("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    if (shader_program == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return -1;
    }

    // Create a simple cube model for testing|debugging
    cube_model = model_create_cube();
    if (cube_model.mesh_count == 0) {
        fprintf(stderr, "Failed to create cube model\n");
        return -1;
    }


    // Load girl model
    printf("Loading girl model...\n");
    girl_model = model_load("assets/models/girl.obj");
    if (girl_model.mesh_count == 0) {
        printf("Warning: Failed to load girl model, using cube instead\n");
        current_model = 0;
    } else {
        printf("Girl model loaded successfully with %u meshes\n", girl_model.mesh_count);
        current_model = 1; // Start with girl model
    }

    // Create a simple white texture for models without textures
    printf("Creating default white texture\n");
    unsigned char white_pixel[] = {255, 255, 255, 255};
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return 0;
}

static void update_engine(void) {
    input_update(window);
    process_input();
    camera_process_input(&camera, window, (float)delta_time);
    physics_step_simulation(&physics_world, (float)delta_time);

    static double last_audio_update = 0.0;
    double current_time = glfwGetTime();
    if (current_time - last_audio_update > 0.1) { // Update 10 times per second
        float listener_pos[3] = {camera.pos[0], camera.pos[1], camera.pos[2]};
        float listener_forward[3] = {camera.front[0], camera.front[1], camera.front[2]};
        float listener_up[3] = {camera.up[0], camera.up[1], camera.up[2]};
        audio_set_listener(listener_pos, listener_forward, listener_up);
        last_audio_update = current_time;
    }
}

static void render_engine(void) {
    mat4 projection;
    glm_perspective(glm_rad(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f, projection);

    renderer_begin_frame(&camera, projection);

    model_t* active_model = (current_model == 1 && girl_model.mesh_count > 0) ? &girl_model : &cube_model;

    mat4 model_matrix;
    glm_mat4_identity(model_matrix);

    if (current_model == 1) {
        glm_translate(model_matrix, (vec3){0.0f, -1.0f, 0.0f}); // Lower to ground
        glm_scale(model_matrix, (vec3){1.0f, 1.0f, 1.0f});

        // Simple animated rotation
        glm_rotate(model_matrix, (float)glfwGetTime() * glm_rad(30.0f), (vec3){0.0f, 1.0f, 0.0f});
    } else {
        // Cube with physics
        vec3 physics_pos;
        mat4 physics_rotation;
        physics_get_transform(physics_cube, physics_pos, physics_rotation);
        glm_translate(model_matrix, physics_pos);
        glm_mat4_mul(model_matrix, physics_rotation, model_matrix);
    }

    shader_use(shader_program);
    shader_set_vec3(shader_program, "lightPos", light_pos);
    shader_set_vec3(shader_program, "lightColor", light_color);
    shader_set_vec3(shader_program, "viewPos", camera.pos);
    shader_set_int(shader_program, "texture_diffuse1", 0);

    // Bind fallback texture (for meshes without textures)
    texture_bind(texture_id, 0);

    renderer_submit(*active_model, model_matrix, shader_program);
    renderer_end_frame();
}

static void cleanup_engine(void) {
    model_free(&cube_model);
    model_free(&girl_model);

    if (shader_program != 0) {
        shader_delete(shader_program);
    }

    if (texture_id != 0) {
        texture_delete(texture_id);
    }

    physics_world_destroy(&physics_world);
    if (kaleidoscope != 0) audio_delete_buffer(kaleidoscope);
    audio_shutdown();

    if (window) {
        window_terminate();
    }
}

static void process_input(void) {
    if (input_is_key_pressed(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, 1);
    }

    // Toggle wireframe on F1
    static int f1_pressed = 0;
    if (input_is_key_pressed(window, GLFW_KEY_F1)) {
        if (!f1_pressed) {
            static int wireframe = 0;
            wireframe = !wireframe;
            if (wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                printf("Wireframe mode ON\n");
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                printf("Wireframe mode OFF\n");
            }
            f1_pressed = 1;
        }
    } else {
        f1_pressed = 0;
    }

    // M - Toggle between models
    static int m_pressed = 0;
    if (input_is_key_pressed(window, GLFW_KEY_M)) {
        if (!m_pressed) {
            current_model = (current_model == 0) ? 1 : 0;
            if (current_model == 1 && girl_model.mesh_count > 0) {
                printf("Switched to girl model\n");
            } else {
                current_model = 0;
                printf("Switched to cube model\n");
            }
            m_pressed = 1;
        }
    } else {
        m_pressed = 0;
    }

    // 1 - Play audio track
    static int key1_pressed = 0;
    if (input_is_key_pressed(window, GLFW_KEY_1)) {
        if (!key1_pressed && kaleidoscope != 0) {
            audio_play_sound(kaleidoscope);
            printf("Playing audio track Romchika\n");
            key1_pressed = 1;
        }
    } else {
        key1_pressed = 0;
    }
}
