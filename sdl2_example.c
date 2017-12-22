// @NOTE: This is an example of how to do a simple SDL game.
// It's sort of a template but not exactly, should be easy to copy paste to another file and use as the bases for any
// sort of experiments or games.

#include "steve.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

int
main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        exit(-1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow("example",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1024,
                                          768,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_RESIZABLE |
                                          SDL_WINDOW_ALLOW_HIGHDPI);

    if (!window) {
        exit(-1);
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        exit(-1);
    }

    if (!gladLoadGL()) {
        exit(-1);
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        exit(-1);
    }

    SDL_DisplayMode display_mode;
    SDL_GetDisplayMode(0, 0, &display_mode);

    f32 game_update_hz = (f32)display_mode.refresh_rate;
    f32 target_seconds_per_frame = 1.0f / game_update_hz;

    // Setup nanovg
    NVGcontext *vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    if (vg == NULL) {
        exit(-1);
    }

    u64 update_time = 0;
    u64 frame_time = 0;

    u64 last_counter = SDL_GetPerformanceCounter();
    u64 last_start_time = SDL_GetTicks();

    bool running = true;
    while (running) {
        i32 start_time = SDL_GetTicks();
        frame_time = start_time - last_start_time;
        last_start_time = start_time;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    running = false;
                } break;
                default:
                    break;
            }
        }

        i32 display_w, display_h, width, height;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GL_GetDrawableSize(window, &display_w, &display_h);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        nvgBeginFrame(vg, width, height, display_w/width);

        nvgBeginPath(vg);
        nvgFillColor(vg, nvgRGBf(1,0,0));
        nvgRect(vg, 10, 10, 10, 10);
        nvgFill(vg);

        nvgEndFrame(vg);

        update_time = SDL_GetTicks() - start_time;
        f64 time_till_vsync = target_seconds_per_frame * 1000.0 - (SDL_GetTicks() - start_time);
        if (time_till_vsync > 4) {
            SDL_Delay((u32)(time_till_vsync - 3));
        }

        SDL_GL_SwapWindow(window);

        u64 end_counter = SDL_GetPerformanceCounter();
        last_counter = end_counter;
    }

    SDL_Quit();
    return(0);
}