cmake_minimum_required(VERSION 3.16)
project(cv_playground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# Fetch Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.89.2
)
FetchContent_MakeAvailable(imgui)

# Fetch stb (single-header libraries)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# Fetch xtl (dependency of xtensor)
FetchContent_Declare(
    xtl
    GIT_REPOSITORY https://github.com/xtensor-stack/xtl.git
    GIT_TAG 0.8.0
)
set(XTL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(xtl)

# Fetch xtensor (header-only)
FetchContent_Declare(
    xtensor
    GIT_REPOSITORY https://github.com/xtensor-stack/xtensor.git
    GIT_TAG 0.26.0
)
set(XTENSOR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(xtensor)

add_executable(cv_playground
    src/main.cpp
    src/image.cpp
)

# Copy data directory after build
add_custom_command(
    TARGET cv_playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
)

# Expose include paths for all fetched content
target_include_directories(cv_playground PRIVATE
    ${glad_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${stb_SOURCE_DIR}            # <- now pulls in stb_image.h, stb_image_write.h, etc.
)

find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(cv_playground PRIVATE
    glfw
    glad
    OpenGL::GL
    imgui_impl
    xtl
    xtensor
)

# ImGui implementation
add_library(imgui_impl STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_impl PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
)

target_link_libraries(cv_playground PRIVATE imgui_impl)