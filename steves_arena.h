/*
Dynamically growable arena.
Uses malloc for allocation. @TODO: Allow a different allocator to be defined.
Expands in chunks based on a default allocation size.
Can be freed all at once.
Supports "temp regions" that let you free a group of allocations at once.

Allocations are 16 byte aligned.

Heavily influenced by handmade hero's arenas.

@TODO: more push helpers, pushCopy and pushString
@TODO: pass in wanted alignment (or at least allow 32 byte so we can easily get arrays on different cache lines)
@TODO: tests for the alignment
@TODO: arena iterator for debug purposes

When do you want to use this?
- When you have temp stuff you want to be all freed at once. (temp memory for calculations)
- When you want all allocations to be linked so you can serialize and reset the whole state. (gamestate)
*/

#ifndef _steves_arena_h
#define _steves_arena_h

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
// # For windows
// #include <malloc.h>

#define MAX_(a, b) ((a > b) ? (a) : (b))

typedef struct MemoryBlockFooter_t {
    uint8_t *base;
    uintptr_t used;
    uintptr_t capacity;

    struct MemoryBlockFooter_t *prev;
} MemoryBlockFooter;

typedef struct {
    uint8_t *base;
    uintptr_t used;
    uintptr_t capacity;
    
    uintptr_t minimum_block_size;
    int temp_count;
} MemoryArena;

typedef struct {
    MemoryArena *arena;
    uint8_t *begin;
} ArenaTempMemory;

#define zeroStruct(instance) zeroSize(sizeof(instance), &(instance))
#define zeroArray(count, ptr) zeroSize(sizeof(ptr[0]) * count, ptr)
void
zeroSize(uintptr_t size, void *ptr)
{
    uint8_t *byte = (uint8_t*)ptr;
    while (size--) {
        *byte++ = 0;
    }
}

#define arenaPushType(a, t) (t*)arenaPushSize(a, sizeof(t), false)
#define arenaPushArray(a, t, n) (t*)arenaPushSize(a, sizeof(t)*n, false)
#define arenaPushEmptyType(a, t) (t*)arenaPushSize(a, sizeof(t), true)
#define arenaPushEmptyArray(a, t, n) (t*)arenaPushSize(a, sizeof(t)*n, true)
void *
arenaPushSize(MemoryArena *arena, uintptr_t size_init, bool clear_to_zero)
{
    uint8_t *result = (uint8_t*)((uint8_t)(arena->base + arena->used) + 15 & ~ (uintptr_t)0x0F);
    uintptr_t align_offset = result - (arena->base + arena->used);
    uint64_t size = size_init + align_offset;

    if (!arena->base ||
        arena->used + size > arena->capacity) {

        size = size_init;
        if (!arena->minimum_block_size) {
            arena->minimum_block_size = 1024 * 1024; // @Q: is 1mb a good default?
        }

        MemoryBlockFooter save;
        save.base = arena->base;
        save.used = arena->used;
        save.capacity = arena->capacity;

        uintptr_t block_size = MAX_(size + sizeof(MemoryBlockFooter), arena->minimum_block_size);

        arena->base = (uint8_t*)malloc(block_size);
        arena->used = 0;
        arena->capacity = block_size - sizeof(MemoryBlockFooter);
        *(MemoryBlockFooter*)(arena->base + arena->capacity) = save;

        result = arena->base;
        align_offset = 0;
    }

    assert(size >= size_init);
    assert(arena->used + size <= arena->capacity);

    arena->used += size;

    assert(((uintptr_t)result & 15) == 0);

    if (clear_to_zero) {
        zeroSize(size_init, result);
    }

    return result;
}

#define arenaBootstrapPushStruct(type, field) arenaBootstrapPushSize(sizeof(type), offsetof(type, field))
void*
arenaBootstrapPushSize(uintptr_t size, uintptr_t offset_to_arena)
{
    MemoryArena bootstrap = {0};
    void *result = arenaPushSize(&bootstrap, size, true);
    *(MemoryArena*)((uint8_t*)result + offset_to_arena) = bootstrap;

    return result;
}

ArenaTempMemory
arenaBeginTempMemory(MemoryArena *arena)
{
    ArenaTempMemory temp_memory;
    
    temp_memory.arena = arena;
    temp_memory.begin = arena->base + arena->used;
    
    arena->temp_count++;

    return temp_memory;
}

void
arenaEndTempMemory(ArenaTempMemory temp_memory)
{    
    MemoryArena *arena = temp_memory.arena;
    while ((temp_memory.begin < arena->base) ||
           (temp_memory.begin > arena->base + arena->capacity)) {
        MemoryBlockFooter footer = *(MemoryBlockFooter*)(arena->base + arena->capacity);
        uint8_t *old_base = arena->base;
        arena->base = footer.base;
        arena->used = footer.used;
        arena->capacity = footer.capacity;
        free(old_base);
    }

    arena->temp_count--;
}

#undef MAX_
#endif // _steves_arena_h