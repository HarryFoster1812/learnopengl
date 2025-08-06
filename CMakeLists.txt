cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include headers
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/glad
    ${PROJECT_SOURCE_DIR}/include/stb
)

# Source files
set(SOURCES
    src/glad/glad.c
    src/stb/stb_image.cpp
    src/core/Shader.cpp
    src/core/Texture.cpp
    Chapter\ 8/main.cpp
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
