/* danielsinkin97@gmail.com */

// Core system and OpenGL
#include <SDL.h>
#include <glad/glad.h>

// ImGui backends
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl.h"
#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Standard library
#include <array>
#include <assert.h>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <thread>

using std::chrono::steady_clock;
using namespace std::chrono_literals;

// Project headers
#include "constants.hpp"
#include "engine.hpp"
#include "gl.hpp"
#include "global.hpp"
#include "input.hpp"
#include "log.hpp"
#include "render.hpp"
#include "types.hpp"
#include "utils.hpp"

auto main(int argc, char **argv) -> int {
    LOG_INFO("Application starting");

    if (!engine_setup()) PANIC("Setup failed!");
    LOG_INFO("Engine setup complete");

    global.renderer.blit_shader = GL::ShaderProgram();
    global.renderer.blit_shader.load(
        Constants::fp_vertex_shader,
        Constants::fp_fragment_shader);
    global.renderer.geom_square = GL::create_geometry(Constants::square_vertices, Constants::square_indices);

    stbi_uc *image_data = stbi_load(
        Constants::fp_image_hummingbird,
        &global.renderer.image_texture.width,
        &global.renderer.image_texture.height,
        &global.renderer.image_texture.channels,
        STBI_rgb_alpha);
    if (!image_data) LOG_ERR("Failed to load image {}. (Reason:{})", Constants::fp_image_hummingbird, stbi_failure_reason());

    glGenTextures(1, &global.renderer.image_texture.id);
    glBindTexture(GL_TEXTURE_2D, global.renderer.image_texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        global.renderer.image_texture.width,
        global.renderer.image_texture.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image_data);

    global.is_running = true;
    global.sim.run_start_time = std::chrono::steady_clock::now();
    global.sim.frame_start_time = global.sim.run_start_time;
    auto last_instruction_time = std::chrono::steady_clock::now();
    LOG_INFO("Entering main loop");
    while (global.is_running) {
        auto now = std::chrono::steady_clock::now();
        global.sim.delta_time = now - global.sim.frame_start_time;
        global.sim.frame_start_time = now;
        global.sim.total_runtime = now - global.sim.run_start_time;

        handle_input();
        Render::gui_debug();
        Render::frame();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(global.renderer.window);

        global.sim.frame_counter += 1;
    }

    LOG_INFO("Main loop exited");
    engine_cleanup();
    LOG_INFO("Engine cleanup complete");
    LOG_INFO("Application exiting successfully");

    return EXIT_SUCCESS;
}