#include "steve.h"
#include "imgui.h"

#define UPDATE_AND_RENDER(name) void* name(void* memory)
typedef UPDATE_AND_RENDER(UpdateAndRender);
extern "C" UpdateAndRender updateAndRender;