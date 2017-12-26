#include "imgui_example.h"

typedef struct {
    int foo;
} GameState;

extern "C" UPDATE_AND_RENDER(updateAndRender) {
    GameState *gamestate = (GameState*)memory;
    if (gamestate == NULL) {
        gamestate = (GameState*)malloc(sizeof(*gamestate));
    }

    ImGui::Text("frack");

    return gamestate;
}