#include "steve.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
    uint8_t r, g, b, a;
} Color;

Color
color(float r, float g, float b)
{
    Color color = {.r = 255/r,
                   .g = 255/g,
                   .b = 255/b,
                   .a = 255};
    return color;
}

int
main(void) {
    int width = 300;
    int height = 300;
    Color data[width*height];

    logInfo("Generating Map");
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            data[y*width + x] = color(1.0, 0.0, 0.0);
        }
    }
    
    logInfo("Writing Map to map.png");
    stbi_write_png("map.png", width, height, 4, data, sizeof(Color)*width);

    logInfo("Done");
    return(0);
}