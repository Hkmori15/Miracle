#ifndef MODEL_H
#define MODEL_H

#include <cglm/cglm.h>

typedef struct {
    vec3 pos;
    vec3 norm;
    vec3 tex;
} vertex_t;

typedef struct {
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int index_count;
    unsigned int diffuse_texture;
    unsigned int normal_texture;
    unsigned int specular_texture;
    char material_name[256];
} mesh_t;

typedef struct {
    mesh_t* meshes;
    unsigned int mesh_count;
} model_t;

/**
    * Load a 3D model from file using library Assimp
    * @param path Path to model file
    * @return Loaded model structure
**/

model_t model_load(const char* path);

/**
    * Draw a model using the specified shader
    * @param model Model to draw
    * @param shader_id Shader program ID
**/

void model_draw(model_t model, unsigned int shader_id);

/**
    * Free model resources
    * @param model Model to free
**/

void model_free(model_t* model);

/**
    * Create a simple cube primitive for testing
    * @return Cube model
**/

model_t model_create_cube(void);

/**
    * Load texture for a material by searching common patterns
    * @param material_name Material name from model
    * @param texture_type Type suffix: "Diffuse", "Normal", "Specular"
    * @return Texture ID or 0 if not found
**/

unsigned int model_load_material_texture(const char* material_name, const char* texture_type);

#endif // MODEL_H
