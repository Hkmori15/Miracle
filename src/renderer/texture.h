#ifndef TEXTURE_H
#define TEXTURE_H

/**
   * Create a texture from an image file
   * @param path Path to image file
   * @return Texture ID on success and 0 on failure
**/

unsigned int texture_create(const char* path);

/**
   * Bind a texture for use
   * @param id Texture ID
   * @param unit Texture unit 0..31
**/

void texture_bind(unsigned int id, unsigned int unit);

/**
   * Delete a texture
   * @param id Texture ID
**/

void texture_delete(unsigned int id);

#endif // TEXTURE_H
