#pragma once
#include <glm/glm.hpp>

struct MouseState {
    bool firstMouse = true;
    glm::vec2 lastPos = {400.0f, 300.0f};
    glm::vec2 pos     = {400.0f, 300.0f};
    bool leftMousePressed  = false;
    bool rightMousePressed = false;
};
