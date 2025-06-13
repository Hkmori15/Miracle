#ifndef SHADER_H
#define SHADER_H

#include <cglm/cglm.h>

/**
   * Create a shader program from vertex and fragment shader files
   * @param vert_path Path to vertex shader file
   * @param frag_path Path to fragment shader file
   * @return Shader program ID on success and 0 on failure
**/

unsigned int shader_create(const char* vert_path, const char* frag_path);

/**
   * Use/activate a shader program
   * @param id Shader program ID
**/

void shader_use(unsigned int id);

/**
   * Set a mat4 uniform in the shader
   * @param id Shader program ID
   * @param name Uniform variable name
   * @param mat 4x4 matrix to set
**/

void shader_set_mat4(unsigned int id, const char* name, mat4 mat);

/**
   * Set a vec3 uniform in the shader
   * @param id Shader program ID
   * @param name Uniform variable name
   * @param vec 2D vector to set
**/

void shader_set_vec2(unsigned int id, const char* name, vec2 vec);

/**
   * Set a vec3 uniform in the shader
   * @param id Shader program ID
   * @param name Uniform variable name
   * @param vec 3D vector to set
**/

void shader_set_vec3(unsigned int id, const char* name, vec3 vec);

/**
   * Set an int uniform in the shader
   * @param id Shader program ID
   * @param name Uniform variable name
   * @param value int value to set
**/

void shader_set_int(unsigned int id, const char* name, int value);

/**
   * Set a float uniform in the shader
   * @param id Shader program ID
   * @param name Uniform variable name
   * @param value float value to set 
**/

void shader_set_float(unsigned int id, const char* name, float value);

/**
   * Delete a shader program
   * @param id Shader program ID
**/

void shader_delete(unsigned int id);

#endif // SHADER_H
