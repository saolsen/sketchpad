#include <GL/gl3w.h>
#include <SDL2/SDL.h>

#include "forest.h"
#include "steves_reloader.h"

#include <imgui_impl_sdl_gl3.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

NVGcontext *vg;

int
main(int argc, char **argv)
{
    char *base_path = SDL_GetBasePath();
    char trace_file_buf[256];
    snprintf(trace_file_buf, 256, "%s%s", base_path, "../../../forest_trace.json");

    mtr_init(trace_file_buf);
    MTR_META_PROCESS_NAME("forest");
    MTR_META_THREAD_NAME("main thread");

    MTR_BEGIN("main", "setup");

    UpdateAndRender *updateAndRender;

    FunctionSpec function = {0};
    function.function_name = "updateAndRender";
    function.function_ptr = (void**)&updateAndRender;

    LibrarySpec spec = {0};

    char dll_file_buf[256];
    snprintf(dll_file_buf, 256, "%s%s", base_path, "libforest.dylib");

    spec.library_file = dll_file_buf;
    spec.num_functions = 1;
    spec.functions = &function;

    Library library = libraryInit(spec);
    if (!libraryReload(&library)) {
        logError("Could not load library!");
        exit(-1);
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        exit(-1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow("forest",
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

    if (gl3wInit()) {
		logError("failed to initialize OpenGL");
		return -1;
	}
	if (!gl3wIsSupported(3, 3)) {
		logError("OpenGL 3.3 not supported");
		return -1;
	}

    if (SDL_GL_SetSwapInterval(1) < 0) {
        exit(-1);
    }

    SDL_DisplayMode display_mode;
    SDL_GetDisplayMode(0, 0, &display_mode);

    f32 game_update_hz = (f32)display_mode.refresh_rate;
    f32 target_seconds_per_frame = 1.0f / game_update_hz;

    // Setup imgui
    ImGui_ImplSdlGL3_Init(window);

    // Setup nanovg
    // Setup nanovg
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    if (vg == NULL) {
        exit(-1);
    }

    u64 update_time = 0;
    u64 frame_time = 0;

    u64 last_counter = SDL_GetPerformanceCounter();
    u64 last_start_time = SDL_GetTicks();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    void *memory = NULL;

    bool running = true;

    MTR_END("main", "setup");

    while (running) {
        MTR_BEGIN("main", "update");
        i32 start_time = SDL_GetTicks();
        frame_time = start_time - last_start_time;
        last_start_time = start_time;

        libraryReload(&library);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
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

        ImGui_ImplSdlGL3_NewFrame(window);

        nvgBeginFrame(vg, width, height, display_w/width);

        memory = updateAndRender(memory);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        nvgEndFrame(vg);
        //glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        ImGui::Render();
        MTR_END("main", "update");
        mtr_flush();

        update_time = SDL_GetTicks() - start_time;
        f64 time_till_vsync = target_seconds_per_frame * 1000.0 - (SDL_GetTicks() - start_time);
        if (time_till_vsync > 4) {
            SDL_Delay((u32)(time_till_vsync - 3));
        }

        SDL_GL_SwapWindow(window);

        u64 end_counter = SDL_GetPerformanceCounter();
        last_counter = end_counter;
    }
    mtr_flush();
    mtr_shutdown();
    SDL_Quit();
    return(0);
}