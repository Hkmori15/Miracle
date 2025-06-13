#include "shader.h"
#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>

static char* read_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");

    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filepath);

        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer and read file
    char* content = malloc(size + 1);

    if (!content) {
        fprintf(stderr, "Failed to allocate mem for shader file\n");
        fclose(file);

        return NULL;
    }

    fread(content, 1, size, file);
    
    content[size] = '\0'; // null terminate string
    fclose(file);

    return content;
}

/**
    * Helper func to check shader complilation errors
**/

static int check_compile_errors(unsigned int shader, const char* type) {
    int success;
    char info_log[1024];

    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "Shader compilation error of type: %s\n%s\n", type, info_log);

            return 0;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            fprintf(stderr, "Program linking error of type: %s\n%s\n", type, info_log);

            return 0;
        }
    }

    return 1;
}

unsigned int shader_create(const char *vert_path, const char *frag_path) {
    char* vertex_code = read_file(vert_path);
    char* fragment_code = read_file(frag_path);

    if (!vertex_code || !fragment_code) {
        free(vertex_code);
        free(fragment_code);

        return 0;
    }

    // Compile vertex and fragment shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const char**)&vertex_code, NULL);
    glCompileShader(vertex);

    if (!check_compile_errors(vertex, "VERTEX")) {
        free(vertex_code);
        free(fragment_code);
        glDeleteShader(vertex);

        return 0;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const char**)&fragment_code, NULL);
    glCompileShader(fragment);

    if (!check_compile_errors(fragment, "FRAGMENT")) {
        free(vertex_code);
        free(fragment_code);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return 0;
    }

    // Now create shader program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    if (!check_compile_errors(program, "PROGRAM")) {
        free(vertex_code);
        free(fragment_code);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(program);

        return 0;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    free(vertex_code);
    free(fragment_code);

    return program;
}

void shader_use(unsigned int id) {
    glUseProgram(id);
}

void shader_set_mat4(unsigned int id, const char *name, mat4 mat) {
    int location = glGetUniformLocation(id, name);

    glUniformMatrix4fv(location, 1, GL_FALSE, (float*)mat);
}

void shader_set_vec2(unsigned int id, const char *name, vec2 vec) {
    int location = glGetUniformLocation(id, name);

    glUniform2fv(location, 1, vec);
}

void shader_set_vec3(unsigned int id, const char *name, vec3 vec) {
    int location = glGetUniformLocation(id, name);

    glUniform3fv(location, 1, vec);
}

void shader_set_int(unsigned int id, const char *name, int value) {
    int location = glGetUniformLocation(id, name);

    glUniform1i(location, value);
}

void shader_set_float(unsigned int id, const char *name, float value) {
    int location = glGetUniformLocation(id, name);

    glUniform1f(location, value);
}

void shader_delete(unsigned int id) {
    glDeleteProgram(id);
}