#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This is a way to do a dynamically growing array. It manages the buffer internally.
// It's also size agnostic so you can use it for any type!
// Since the first field is a pointer to the elements you can do a union of the Array
// and a pointer of whatever pointer type you want. Then you can access them typed through the pointer
// and call the arrayPush method when you push (making it reusable)

// You can that way use macros to define the type.

typedef struct {
    char *data;
    size_t num_items;
    size_t max_items;
} Array;

void arrayInit(Array *a, size_t item_size)
{
    a->num_items = 0;
    a->max_items = 16;
    a->data = malloc(a->max_items * item_size);
}

// Copies the contents of item onto the end of the array.
void arrayPush(Array *a, void *item, size_t item_size)
{
    if (a->num_items == a->max_items) {
        a->max_items *= 2;
        a->data = realloc(a->data, a->max_items * item_size);
    }
    memcpy(a->data + a->num_items*item_size, item, item_size);
    a->num_items++;
}

void arrayFree(Array *a)
{
    free(a->data);
}

// Here's how to make like an array of a type.
typedef struct {
    union {
        struct {
            float *vertices;
            size_t num_vertices;
        };
        Array vertices_array;
    };
} VertexList;

void vertexListInit(VertexList *vertex_list)
{
    arrayInit(&vertex_list->vertices_array, sizeof(*vertex_list->vertices));
}

void vertexListPush(VertexList *vertex_list, float f)
{
    arrayPush(&vertex_list->vertices_array, &f, sizeof(*vertex_list->vertices));
}

void vertexListFree(VertexList *vertex_list)
{
    arrayFree(&vertex_list->vertices_array);
}

// Could you do some nice macros to create this? Would that be a good idea even?
// Can I do this same thing for a packed array?

int main() {
    printf("Hello array thingy\n");

    VertexList list;
    vertexListInit(&list);

    for (int i=0; i<100; i++) {
        float x = i;
        vertexListPush(&list, x);
    }

    vertexListFree(&list);
}
