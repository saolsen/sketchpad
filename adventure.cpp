#include "adventure.h"
#include "steves_arena.h"

#include "pcg_variants.h"

#include <tgmath.h>

// THE WAY (according to sean)
// He also code generates this stuff.
// Can have additional every entity stuff up next to type.
// Not space efficiant, wasted on the union and wasted if you're ever
// looping over just one type.
// You could pull stuff out of here into additional arrays and use ids/handles
// to reference them when you do want really fast loops over like animation data
// or positions or whatever.
typedef enum {
    FOO,
    BAR,
    BAZ,
    NUM_TYPES
} Type;

typedef union {
    Type type;
    struct {
        Type type;
        u32 foo;
    } foo;
    struct {
        Type type;
        float bar;
    } bar;
    struct {
        Type type;
        i32 baz;
    } baz;
} Entity;

typedef struct {
    MemoryArena *arena;
    pcg32_random_t rng;

    Entity entities[32];
    u32 num_entities;
} GameState;

extern "C" UPDATE_AND_RENDER(updateAndRender) {
    GameState *gamestate = (GameState*)memory;
    if (gamestate == NULL) {
        gamestate = arenaBootstrapPushStruct(GameState, arena);

        // @TODO: Random seed.
        pcg32_srandom_r(&gamestate->rng, 0u, 0u);

        // Generate some random entities.
        for (int i=0; i<LEN(gamestate->entities); i++) {
            Entity *entity = gamestate->entities + i;
            entity->type = (Type)pcg32_boundedrand_r(&gamestate->rng, NUM_TYPES);
            switch(entity->type) {
                case(FOO): {
                    entity->foo.foo = pcg32_random_r(&gamestate->rng);
                } break;
                case(BAR): {
                    entity->bar.bar = ldexp(pcg32_random_r(&gamestate->rng), -32);
                } break;
                case (BAZ): {
                    entity->baz.baz = pcg32_random_r(&gamestate->rng);
                } break;
                case (NUM_TYPES): break;
            };
        }
    }

    // I really want to figure out how the fuck you do that custom drawing stuff with imgui. Want that for debug views
    // of some cool algortihmic stuff or whatever.
    // ImGui::ShowDemoWindow();
    ImGui::Begin("Entities");
    ImGui::Text("Entities");
    for (int i=0; i<LEN(gamestate->entities); i++) {
        Entity *entity = gamestate->entities + i;
        switch(entity->type) {
            case(FOO): {
                ImGui::Text("Foo");
                ImGui::SameLine();
                ImGui::Text("%" PRIu32, entity->foo.foo);
            } break;
            case(BAR): {
                ImGui::Text("Bar");
                ImGui::SameLine();
                ImGui::Text("%f", entity->bar.bar);
            } break;
            case (BAZ): {
                ImGui::Text("Baz");
                ImGui::SameLine();
                ImGui::Text("%i", entity->baz.baz);
            } break;
            case (NUM_TYPES): break;
        };
    }
    ImGui::End();

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBf(1,0,0));
    nvgRect(vg, 0, 0, 10, 10);
    nvgFill(vg);

    return gamestate;
}