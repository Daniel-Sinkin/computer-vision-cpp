/* danielsinkin97@gmail.com */
#define GLFW_INCLUDE_NONE
#include "globals.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "image.hpp"
#include "stb_image.h"

#include <iostream>

auto setup() -> int {
    if (glfwInit() == GLFW_FALSE) return false;

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    globals.window = glfwCreateWindow(
        Constants::window_width,
        Constants::window_height,
        Constants::window_title,
        nullptr,
        nullptr);
    if (globals.window == nullptr) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(globals.window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(globals.window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    return true;
}

auto cleanup() -> void {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(globals.window);
    glfwTerminate();
}

auto ImGui_RightAlignedText(const char *text) -> void {
    float right_align_pos = ImGui::GetWindowWidth() - ImGui::CalcTextSize(text).x - 10.0f;
    ImGui::SameLine(right_align_pos);
    ImGui::Text("%s", text);
}

auto handle_command(const char *command) -> void {
    if (strlen(command) == 0) return;
    std::cout << "[Command]: " << command << "\n";
    if (strcmp(command, "quit") == 0) {
        glfwSetWindowShouldClose(globals.window, GLFW_TRUE);
    } else if (strcmp(command, "log") == 0) {
        std::cout << "We would do something like logging here" << "\n";
    } else if (strcmp(command, "mouse") == 0) {
        globals.status_bar_mouse = !globals.status_bar_mouse;
    } else if (strcmp(command, "version") == 0) {
        globals.status_bar_version = !globals.status_bar_version;
    } else if (strcmp(command, "fps") == 0) {
        globals.status_bar_fps = !globals.status_bar_fps;
    } else {
        std::cerr << "Command is invalid!" << "\n";
    }
}

auto main_render_imgui() -> void {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    { // Display Settings
        ImGui::Begin("Display Settings");
        ImGui::ColorEdit3("Background Color", globals.background_color.rgb());
        ImGui::ColorEdit3("Dear ImGui Window BG", globals.ui_window_bg_color.rgb());
        ImGui::ColorEdit3("Dear ImGui Text", globals.ui_text_color.rgb());
        ImGuiStyle &style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(
            globals.ui_window_bg_color.r,
            globals.ui_window_bg_color.g,
            globals.ui_window_bg_color.b,
            globals.ui_window_bg_color.a);
        style.Colors[ImGuiCol_Text] = ImVec4(
            globals.ui_text_color.r,
            globals.ui_text_color.g,
            globals.ui_text_color.b,
            globals.ui_text_color.a);

        ImGui::End();
    } // Display Settings

    { // Image Window
        ImGui::Begin("Image Viewer");
        ImGui::Text(
            "Hummingbird Image: Size: %d x %d, Channels: %d",
            globals.hummingbird_image->w(),
            globals.hummingbird_image->h(),
            globals.hummingbird_image->c());

        if (globals.hummingbird_texture != 0) {
            ImGui::Image(
                reinterpret_cast<void *>(static_cast<intptr_t>(globals.hummingbird_texture)),
                ImVec2(512.0f, 512.0f));
        }
        ImGui::End();
    } // Image Window

    { // Status Bar
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - 24));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 24));
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("StatusBar", nullptr, flags);
        if (globals.status_bar_fps) {
            ImGui::Text("%.4f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
            ImGui::SameLine();
        }
        { // Command Buffer Input
            ImGui::PushItemWidth(400.0f);
            if (globals.focus_command_input) {
                ImGui::SetKeyboardFocusHere();
                globals.focus_command_input = false;
            }
            bool command_executed = (ImGui::InputText(
                "##command_input",
                globals.command_buffer,
                IM_ARRAYSIZE(globals.command_buffer),
                ImGuiInputTextFlags_EnterReturnsTrue));
            if (command_executed) {
                handle_command(globals.command_buffer);
                globals.command_buffer[0] = '\0';
                globals.focus_command_input = true;
            }
            ImGui::PopItemWidth();
        } // Command Buffer Input
        ImGui::SameLine();
        ImGui::Text("%zu / %zu", strlen(globals.command_buffer), Constants::command_buffer_size);
        ImGui::SameLine();
        if (globals.status_bar_mouse) {
            if (glfwGetWindowAttrib(globals.window, GLFW_HOVERED)) {
                ImGui::Text("Mouse: (%.0f, %.0f)", globals.mouse_x, globals.mouse_y);
            } else {
                ImGui::Text("Mouse: ");
            }
        }
        if (globals.status_bar_version) {
            ImGui_RightAlignedText("Version 0.0.alpha");
        }
        ImGui::End();
    } // Status Bar
    ImGui::Render();
};

auto main_handle_input() -> void {
    glfwPollEvents();
    if (glfwGetKey(globals.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(globals.window, GLFW_TRUE);
    }
    if (glfwGetKey(globals.window, GLFW_KEY_F1) == GLFW_PRESS) {
        globals.focus_command_input = true;
    }
    glfwGetCursorPos(globals.window, &globals.mouse_x, &globals.mouse_y);
}

auto bind_image_to_texture(const Image &image) -> GLuint {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        image.w(),
        image.h(),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image.pixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

auto main(int argc, char **argv) -> int {
    if (setup() != true) return -1;

    std::string image_path = std::string(constants.folder_images) + constants.image_name_hummingbird;
    try {
        globals.hummingbird_image = std::make_unique<Image>(image_path);
        globals.hummingbird_texture = bind_image_to_texture(*globals.hummingbird_image);

        std::cout << "Loaded hummingbird: "
                  << globals.hummingbird_image->w() << "x" << globals.hummingbird_image->h()
                  << ", " << globals.hummingbird_image->c() << " channels\n";
    } catch (const std::exception &exc) {
        std::cerr << "Failed to load image " << image_path << ": " << exc.what() << "\n";
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(globals.window)) {
        main_handle_input();
        main_render_imgui();

        int display_w, display_h;
        glfwGetFramebufferSize(globals.window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);

        glClearColor(
            globals.background_color.r,
            globals.background_color.g,
            globals.background_color.b,
            globals.background_color.a);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(globals.window);
    }

    cleanup();
    return 0;
}