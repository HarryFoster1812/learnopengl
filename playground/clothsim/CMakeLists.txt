cmake_minimum_required(VERSION 3.10)
project(main LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define MAIN_PATH for the main.cpp file
set(MAIN_PATH "" CACHE STRING "Path to the main.cpp file to compile")

# Error if MAIN_PATH is not set
if(NOT MAIN_PATH)
    message(FATAL_ERROR "MAIN_PATH is not set. Please pass -DMAIN_PATH=...")
endif()

# Include directories
include_directories(
    ../../include
    ../../include/core
    ../../include/glad
    ../../include/stb
    ../../include/imgui
    ./src
)

# Gather source files
file(GLOB CORE_SOURCES ../../src/core/*.cpp)
file(GLOB MAIN_SOURCES ./src/*.cpp)
file(GLOB IMGUI_SOURCES ../../include/imgui/*.cpp)
file(GLOB IMGUI_BACKEND_SOURCES ../../include/imgui/backends/*.cpp)

# Source files
set(SOURCES
    ../../src/glad/glad.c
    ../../src/stb/stb_image.cpp
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${CORE_SOURCES}
    ${MAIN_SOURCES}
    ${MAIN_PATH}
)

# Create executable
add_executable(main ${SOURCES})

# Find and link libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(main
    PRIVATE
    OpenGL::GL
    glfw
    glm::glm-header-only
    assimp::assimp
)

# Enable ASan and debug symbols for Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(main PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(main PRIVATE -fsanitize=address)
endif()
