#include "forest.h"
#include "steves_arena.h"

typedef struct {
    MemoryArena *arena; // Do I even really want or need an arena?
    pcg32_random_t rng;


} GameState;

// What's the best first experiment?

extern "C" UPDATE_AND_RENDER(updateAndRender) {
    GameState *gamestate = (GameState*)memory;
    if (gamestate == NULL) {
        gamestate = arenaBootstrapPushStruct(GameState, arena);

        // @TODO: Random seed.
        pcg32_srandom_r(&gamestate->rng, 0u, 0u);
    }

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBf(1,1,0));
    nvgRect(vg, 100, 100, 100, 100);
    nvgFill(vg);

    return gamestate;
}