#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct Constants {
    static constexpr int window_width = 1280;
    static constexpr int window_height = 720;
    static constexpr const char *window_title = "Computer Vision";

    static constexpr size_t command_buffer_size = 48;

    static constexpr int my_int = 0;
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

    double mouse_x = 0.0;
    double mouse_y = 0.0;

    char command_buffer[Constants::command_buffer_size + 1] = ""; // + 1 for the null terminator

    bool focus_command_input = true;
};

inline Globals globals;