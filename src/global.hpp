#pragma once

#include <SDL.h>
#include <chrono>
#include <imgui.h>

#include "constants.hpp"
#include "gl.hpp"
#include "types.hpp"

struct RendererState {
    SDL_Window *window = nullptr;
    SDL_GLContext gl_context = nullptr;
    ImGuiIO imgui_io;

    GL::GeometryBuffers geom_square;
    GL::GeometryBuffers geom_circle;
    GL::GeometryBuffers geom_triangle;

    GL::GeometryBuffers blit_quad;
    GL::ShaderProgram blit_shader;

    GL::ImageTexture image_texture;

    int gl_success;
    char gl_error_buffer[512];

    auto panic_gl(const char *reason) -> void {
        std::cerr << reason << "\n"
                  << gl_error_buffer << "\n";
        PANIC("GL Error");
    }
};

struct SimulationState {
    int frame_counter = 0;
    std::chrono::steady_clock::time_point run_start_time;
    std::chrono::steady_clock::time_point frame_start_time;
    std::chrono::duration<float> delta_time;
    std::chrono::duration<float> total_runtime;
};

struct InputState {
    Position mouse_pos;
};

struct ColorPalette {
    Color background = color_from_u8(15, 15, 21);
};

struct Global {
    bool is_running = false;
    RendererState renderer;
    SimulationState sim;
    InputState input;
    ColorPalette color;
};
inline Global global;