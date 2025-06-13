#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"
#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>

static int stb_init = 0;

unsigned int texture_create(const char *path) {
    if (!stb_init) {
        stbi_set_flip_vertically_on_load(1);
        stb_init = 1;
    }

    // Load image data
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

    if (!data) {
        fprintf(stderr, "Failed to load texture: %s\n", path);

        return 0;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Wrapping parameters for texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering parameters for texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine format based on number of channels
    GLenum format;

    switch (channels) {
        case 1:
            format = GL_RED;

            break;

        case 3:
            format = GL_RGB;

            break;

        case 4:
            format = GL_RGBA;

            break;

        default:
            fprintf(stderr, "Unsupported number of channels: %d\n", channels);

            stbi_image_free(data);
            glDeleteTextures(1, &texture);

            return 0;
    }

    // Now upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    printf("Loaded texture: %s | %d%d, %d channels\n", path, width, height, channels);

    return texture;
}

void texture_bind(unsigned int id, unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void texture_delete(unsigned int id) {
    glDeleteTextures(1, &id);
}