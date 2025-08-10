cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MAIN_PATH "" CACHE STRING "Path to the main.cpp file to compile")

# Error if no path is set
if (NOT MAIN_PATH)
    message(FATAL_ERROR "MAIN_PATH is not set. Please pass -DMAIN_PATH=...")
endif()

# Include headers
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/glad
    ${PROJECT_SOURCE_DIR}/include/stb
    ${PROJECT_SOURCE_DIR}/include/imgui
)

file(GLOB CORE_SOURCES src/core/*.cpp)
file(GLOB IMGUI_SOIRCES ${PROJECT_SOURCE_DIR}/include/imgui/*.cpp)
file(GLOB IMGUI_BACKEND_SOIRCES ${PROJECT_SOURCE_DIR}/include/imgui/backends/*.cpp)

# Source files
set(SOURCES
    src/glad/glad.c
    src/stb/stb_image.cpp
    ${IMGUI_SOIRCES}
    ${IMGUI_BACKEND_SOIRCES}
    ${CORE_SOURCES}
    ${MAIN_PATH}
)

add_executable(main ${SOURCES})

# Link OpenGL and GLFW
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(main
    OpenGL::GL
    glfw
    glm::glm-header-only
)
