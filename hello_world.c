#include "steve.h"
#include "steves_arena.h"

typedef struct {
    MemoryArena *arena;
    
    uint8_t *clouds;
    size_t num_clouds;
    size_t max_clouds;
} State;

typedef struct {
    int facebook;
} Foo;

int
main() {
    logInfo("Hello World");

    State *state = arenaBootstrapPushStruct(State, arena);
    state->num_clouds = 0;
    state->max_clouds = 10;
    state->clouds = arenaPushArray(state->arena, uint8_t, state->max_clouds);
    zeroArray(state->max_clouds, state->clouds);
    
    // I do wish to learn how to use libcurl from c....
    // I also want to know how to use
    // - postgres
    // - sqlite
    // - other libraries, libraries are good if they are good libraries!
    // - asyncio!
    // - worker queue / task queue, multithreading stuff!
}
