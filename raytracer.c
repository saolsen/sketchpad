#include "steve.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef union {
    struct {f32 x; f32 y; f32 z;};
    float e[3];
} V3;

V3
v3(f32 x, f32 y, f32 z)
{
    return (V3){.x=x, .y=y, .z=z};
}

V3
v3Add(V3 a, V3 b)
{
    return (V3){a.x+b.x, a.y+b.y, a.z+b.z};
}

V3
v3Sub(V3 a, V3 b)
{
    return (V3){a.x-b.x, a.y-b.y, a.z-b.z};
}

V3
v3Scale(V3 v, f32 f)
{
    return (V3){v.x*f, v.y*f, v.z*f};
}

typedef struct {
    V3 color;
} Material;

typedef struct {
    V3 normal;
    f32 distance;
    u32 mat_index;
} Plane;

typedef struct {
    V3 point;
    f32 radius;
    u32 mat_index;
} Sphere;

typedef struct {
    u32 material_count;
    Material *materials;

    u32 plane_count;
    Plane *planes;

    u32 sphere_count;
    Sphere *spheres;
} World;

typedef struct {
    uint8_t r, g, b, a;
} Color;

Color
color(f32 r, f32 g, f32 b)
{
    Color color = {.r = 255/r,
                   .g = 255/g,
                   .b = 255/b,
                   .a = 255};
    return color;
}


int
main(void) {
    u32 width = 300;
    u32 height = 300;
    Color data[width*height];

    logInfo("Generating Map");
    for (i32 y=0; y<height; y++) {
        for (i32 x=0; x<width; x++) {
            data[y*width + x] = color(1.0, 0.0, 0.0);
        }
    }
    
    logInfo("Writing Map to map.png");
    stbi_write_png("map.png", width, height, 4, data, sizeof(Color)*width);

    logInfo("Done");

    Material materials[2] = {
        {.color=v3(0,0,0)},
        {.color=v3(1,0,0)}
    };

    Plane plane = {
        .normal = v3(1,0,0),
        .distance = 5,
        .mat_index = 1
    };

    World world = {
        .materials=materials,
        .material_count=LEN(materials),
        .planes = &plane,
        .plane_count = 1,
        .spheres=NULL,
        .sphere_count=0
    };

    return(0);
}