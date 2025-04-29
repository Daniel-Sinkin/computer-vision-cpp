#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <memory>

#include "image.hpp"

/* Contains the Constants and static definitions for the project, won't be adjusted at runtime. */
struct Constants {
    static constexpr int window_width = 1280;
    static constexpr int window_height = 720;
    static constexpr const char *window_title = "Computer Vision";

    static constexpr float image_width = 512.0f;
    static constexpr float image_height = 512.0f;

    static constexpr size_t command_buffer_size = 48;

    static constexpr const char *folder_data = "data/";
    static constexpr const char *folder_images = "data/images/";

    static constexpr const char *image_name_fennec = "fennec.png";
    static constexpr const char *image_name_hummingbird = "hummingbird.png";

    static constexpr size_t channel_red = 0;
    static constexpr size_t channel_green = 0;
    static constexpr size_t channel_blue = 0;
    static constexpr size_t channel_alpha = 0;
};
inline constexpr Constants constants{};

struct Color {
    float r = 0.1f;
    float g = 0.1f;
    float b = 0.1f;
    float a = 1.0f;

    auto rgb() -> float * { return &r; }
    auto rgb() const -> const float * { return &r; }

    auto rgba() -> float * { return &r; }
    auto rgba() const -> const float * { return &r; }
};

struct Globals {
    GLFWwindow *window = nullptr;
    Color background_color;

    Color ui_window_bg_color = Color{0.06f, 0.06f, 0.06f, 1.0f};
    Color ui_text_color = Color{1.00f, 1.00f, 1.00f, 1.0f};

    bool status_bar_version = true;
    bool status_bar_mouse = true;
    bool status_bar_fps = true;

    bool image_active = true;

    double mouse_x = 0.0;
    double mouse_y = 0.0;

    char command_buffer[Constants::command_buffer_size + 1] = ""; // + 1 for the null terminator

    bool focus_command_input = true;

    GLuint hummingbird_texture = 0;
    std::unique_ptr<Image> hummingbird_image;
};
inline Globals globals;