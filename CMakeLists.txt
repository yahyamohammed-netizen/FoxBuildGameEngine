cmake_minimum_required(VERSION 3.16)
project(foxbuild LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===================== SOURCES =====================
set(SOURCES
        main.cpp



        # ImGui core
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp

        # ImGui backends
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/misc/cpp/imgui_stdlib.cpp
        external/stb_image.h



        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/misc/cpp/imgui_stdlib.h


        # tinyfiledialogs
        external/tinyfiledialogs/tinyfiledialogs.c
        Node.h
        Node.cpp
        Render.h
        Render.cpp
)

add_executable(foxbuild ${SOURCES})

# ===================== INCLUDES =====================
target_include_directories(foxbuild PRIVATE
        src

        external/imgui
        external/imgui/backends
        external/tinyfiledialogs
        external/imgui/misc/cpp
        external
)

# ===================== LIBS =====================
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)


target_link_libraries(foxbuild PRIVATE
        OpenGL::GL
        SDL2::SDL2
)


