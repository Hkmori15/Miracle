#include "model.h"
#include "texture.h"
#define GL_GLEXT_PROTOTYPES
#include <assimp/material.h>
#include <assimp/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static mesh_t process_mesh(struct aiMesh* mesh, const struct aiScene* scene);
static void process_node(struct aiNode* node, const struct aiScene* scene, model_t* model, unsigned int* mesh_index);

static void process_node(struct aiNode* node, const struct aiScene* scene, model_t* model, unsigned int* mesh_index) {
    // Process all meshes in this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes[*mesh_index] = process_mesh(mesh, scene);

        (*mesh_index)++;
    }

    // Process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, model, mesh_index);
    }
}

static mesh_t process_mesh(struct aiMesh* mesh, const struct aiScene* scene) {
    mesh_t res = {0};

    // Allocate vertex data
    vertex_t* vertices = malloc(sizeof(vertex_t) * mesh->mNumVertices);

    if (!vertices) {
        fprintf(stderr, "Failed to allocate mem for vertices\n");

        return res;
    }

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertex_t vertex;

        vertex.pos[0] = mesh->mVertices[i].x;
        vertex.pos[1] = mesh->mVertices[i].y;
        vertex.pos[2] = mesh->mVertices[i].z;

        if (mesh->mNormals) {
            vertex.norm[0] = mesh->mNormals[i].x;
            vertex.norm[1] = mesh->mNormals[i].y;
            vertex.norm[2] = mesh->mNormals[i].z;
        } else {
            vertex.norm[0] = 0.0f;
            vertex.norm[1] = 1.0f;
            vertex.norm[2] = 0.0f;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.tex[0] = mesh->mTextureCoords[0][i].x;
            vertex.tex[1] = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.tex[0] = 0.0f;
            vertex.tex[1] = 0.0f;
        }

        vertices[i] = vertex;
    }

    // Process indices
    unsigned int* indices = malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);

    if (!indices) {
        fprintf(stderr, "Failed to allocate mem for indices\n");
        
        free(vertices);

        return res;
    }

    unsigned int index_count = 0;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices[index_count++] = face.mIndices[j];
        }
    }

    // Now create OpenGL obj
    glGenVertexArrays(1, &res.vao);
    glGenBuffers(1, &res.vbo);
    glGenBuffers(1, &res.ebo);
    glBindVertexArray(res.vao);

    glBindBuffer(GL_ARRAY_BUFFER, res.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * mesh->mNumVertices, vertices, GL_STATIC_DRAW);

    // Bind and upload indices to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * index_count, indices, GL_STATIC_DRAW);

    // Set vertex attr pointers
    // pos attr: location = 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attr: location = 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, norm));
    glEnableVertexAttribArray(1);

    // Texture coordinate attr: location = 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    res.index_count = index_count;

    // Process material
    if (mesh->mMaterialIndex >= 0 && scene->mMaterials) {
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        struct aiString material_name;

        if (aiGetMaterialString(material, AI_MATKEY_NAME, &material_name) == AI_SUCCESS) {
            strncpy(res.material_name, material_name.data, sizeof(res.material_name) - 1);

            res.material_name[sizeof(res.material_name) - 1] = '\0';

            printf("Processing material: %s\n", res.material_name);

            // Default|fallback textures
            res.diffuse_texture = 0;
            res.normal_texture = 0;
            res.specular_texture = 0;

            printf("  Using default textures\n");
        } else {
            strcpy(res.material_name, "default_material");
        }
    } else {
        strcpy(res.material_name, "no_material");
    }

    free(vertices);
    free(indices);

    return res;
}

model_t model_load(const char *path) {
    model_t model = {0};

    const struct aiScene* scene = aiImportFile(path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "Assimp error: %s\n", aiGetErrorString());

        return model;
    }

    // Allocate mem for meshes
    model.mesh_count = scene->mNumMeshes;
    model.meshes = malloc(sizeof(mesh_t) * model.mesh_count);

    if (!model.meshes) {
        fprintf(stderr, "Failed to allocate mem for meshes\n");
        aiReleaseImport(scene);

        return model;
    }

    // Process all meshes
    unsigned int mesh_index = 0;
    process_node(scene->mRootNode, scene, &model, &mesh_index);
    aiReleaseImport(scene);

    printf("Loaded model: %s | %u meshes\n", path, model.mesh_count);

    return model;
}

void model_draw(model_t model, unsigned int shader_id) {
    (void)shader_id;

    static int debug_printed = 0;

    if (!debug_printed) {
        printf("Drawing model with %u meshes\n", model.mesh_count);
        debug_printed = 1;
    }

    for (unsigned int i = 0; i < model.mesh_count; i++) {
        mesh_t* mesh = &model.meshes[i];

        if (!debug_printed) {
            printf("Mesh %u: VAO=%u | indices=%u | material=%s\n", i, mesh->vao, mesh->index_count, mesh->material_name);
        }

        // Bind textures if available
        if (mesh->diffuse_texture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh->diffuse_texture);
        }

        if (mesh->normal_texture != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mesh->normal_texture);
        }

        if (mesh->specular_texture != 0) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mesh->specular_texture);
        }

        // Now draw mesh
        glBindVertexArray(mesh->vao);
        glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void model_free(model_t *model) {
    if (model->meshes) {
        for (unsigned int i = 0; i < model->mesh_count; i++) {
            mesh_t* mesh = &model->meshes[i];

            glDeleteVertexArrays(1, &mesh->vao);
            glDeleteBuffers(1, &mesh->vbo);
            glDeleteBuffers(1, &mesh->ebo);

            if (mesh->diffuse_texture != 0) {
                glDeleteTextures(1, &mesh->diffuse_texture);
            }

            if (mesh->normal_texture != 0) {
                glDeleteTextures(1, &mesh->normal_texture);
            }

            if (mesh->specular_texture != 0) {
                glDeleteTextures(1, &mesh->specular_texture);
            }
        }

        free(model->meshes);

        model->meshes = NULL;
        model->mesh_count = 0;
    }
}

model_t model_create_cube(void) {
    model_t model = {0};

    // It's simple cube vertices - each face has it's own vertices
    // for proper normals
    vertex_t vertices[] = {
        // Front face (z = 0.5)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 2
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

        // Back face (z = -0.5)
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 4
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 5
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 6
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 7

        // Left face (x = -0.5)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 8
        {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 9
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 10
        {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 11

        // Right face (x = 0.5)
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 13
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 14
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 15

        // Top face (y = 0.5)
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 17
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 19

        // Bottom face (y = -0.5)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 21
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 22
        {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}  // 23
    };

    unsigned int indices[] = {
        // Front face (CCW)
        0,  1,  2,   2,  3,  0,
        // Back face (CCW)
        4,  5,  6,   6,  7,  4,
        // Left face (CCW)
        8,  9,  10,  10, 11, 8,
        // Right face (CCW)
        12, 13, 14,  14, 15, 12,
        // Top face (CCW)
        16, 17, 18,  18, 19, 16,
        // Bottom face (CCW)
        20, 21, 22,  22, 23, 20
    };

    model.mesh_count = 1;
    model.meshes = malloc(sizeof(mesh_t));

    if (!model.meshes) {
        fprintf(stderr, "Failed to allocate mem for cube mesh\n");

        return model;
    }

    mesh_t* mesh = &model.meshes[0];
    mesh->diffuse_texture = 0;
    mesh->normal_texture = 0;
    mesh->specular_texture = 0;
    mesh->index_count = sizeof(indices) / sizeof(indices[0]);

    strcpy(mesh->material_name, "cube_material");

    printf("Creating cube: %u vertices | %u indices\n", (unsigned int)(sizeof(vertices) / sizeof(vertices[0])), mesh->index_count);

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);
    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, norm));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);

    return model;
}

unsigned int model_load_material_texture(const char *material_name, const char *texture_type) {
    char texture_path[512];
    const char* extensions[] = {".jpg", ".png", ".tga", ".bmp", NULL};

    // Try different naming patterns for loading textures
    const char* patterns[] = {
        "assets/textures/%s_%s%s", // MaterialName_Type.ext
        "assets/textures/%s%s%s", // MaterialNameType.ext
        "assets/textures/%s%s", // MaterialName.ext - ingore type
        NULL
    };

    for (int p = 0; patterns[p] != NULL; p++) {
        for (int e = 0; extensions[e] != NULL; e++) {
            if (p == 2) {
                // For pattern 2 - ignore texture_type
                snprintf(texture_path, sizeof(texture_path), patterns[p], material_name, extensions[e]);
            } else {
                snprintf(texture_path, sizeof(texture_path), patterns[p], material_name, texture_type, extensions[e]);
            }

            struct stat st;

            if (stat(texture_path, &st) == 0) {
                printf("Found texture: %s\n", texture_path);

                return texture_create(texture_path);
            }
        }
    }

    printf("Texture not found for material '%s' | type '%s'\n", material_name, texture_type);

    return 0;
}