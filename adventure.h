#include "steve.h"
#include "imgui.h"
#include "nanovg.h"

// Stuff set by the platform to be used by the game.
extern NVGcontext *vg;

#define UPDATE_AND_RENDER(name) void* name(void* memory)
typedef UPDATE_AND_RENDER(UpdateAndRender);
extern "C" UpdateAndRender updateAndRender;
