#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Simple BMP header structures
#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} bmp_header_t;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} bmp_info_t;
#pragma pack(pop)

void create_test_texture() {
    const int width = 64;
    const int height = 64;
    const int bytes_per_pixel = 3; // RGB
    
    // Create directory
    system("mkdir -p assets/textures");
    
    FILE* file = fopen("assets/textures/default.bmp", "wb");
    if (!file) {
        printf("Failed to create texture file\n");
        return;
    }
    
    // BMP header
    bmp_header_t header = {0};
    header.type = 0x4D42; // "BM"
    header.size = sizeof(bmp_header_t) + sizeof(bmp_info_t) + width * height * bytes_per_pixel;
    header.offset = sizeof(bmp_header_t) + sizeof(bmp_info_t);
    
    // BMP info
    bmp_info_t info = {0};
    info.size = sizeof(bmp_info_t);
    info.width = width;
    info.height = height;
    info.planes = 1;
    info.bits_per_pixel = 24;
    info.compression = 0;
    info.image_size = width * height * bytes_per_pixel;
    
    // Write headers
    fwrite(&header, sizeof(header), 1, file);
    fwrite(&info, sizeof(info), 1, file);
    
    // Create checkerboard pattern
    for (int y = height - 1; y >= 0; y--) { // BMP is bottom-up
        for (int x = 0; x < width; x++) {
            uint8_t color[3];
            if ((x / 8 + y / 8) % 2 == 0) {
                // White
                color[0] = 255; // B
                color[1] = 255; // G
                color[2] = 255; // R
            } else {
                // Orange
                color[0] = 50;  // B
                color[1] = 100; // G
                color[2] = 200; // R
            }
            fwrite(color, 3, 1, file);
        }
    }
    
    fclose(file);
    printf("Created test texture: assets/textures/default.bmp\n");
}

int main() {
    create_test_texture();
}
