#include "Point.hpp"
#include "Mouse.hpp"
#include "Spring.hpp"

// Keep includes sane:
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Point::Point(float x, float y, float z)
    : pos(x, y, z), prevPos(x, y, z), initPos(x, y, z) {}
Point::Point(glm::vec3 position)
    : pos(position), prevPos(position), initPos(position) {}

void Point::setPosition(float x, float y, float z) {
  pos = glm::vec3(x, y, z);
  prevPos = pos;
  initPos = pos;
}

void Point::pin() { isPinned = true; }

void Point::addSpring(Spring *spring) { attachedSprings.push_back(spring); }

void Point::resolveCollisions(const std::vector<CollidableObject *> &objects) {
  for (const auto &object : objects) {
    object->resolveCollision(pos, prevPos);
  }
}

void Point::update(float deltaTime, float drag, const glm::vec3 &acceleration,
                   float elasticity, MouseState &mouseState, int windowWidth,
                   int windowHeight, const glm::mat4 &projection,
                   const glm::mat4 &view, const glm::mat4 &model) {
  glm::vec4 viewport =
      glm::vec4(0.0f, 0.0f, float(windowWidth), float(windowHeight));

  // Project point position to screen space
  glm::vec3 screenPos = glm::project(pos, view * model, projection, viewport);

  glm::vec2 point2D(screenPos.x,
                    float(windowHeight) - screenPos.y); // Flip y if needed

  glm::vec2 mouseDir = point2D - glm::vec2(windowWidth / 2, windowHeight / 2);
  float mouseDist = glm::length(mouseDir);
  isSelected = mouseDist < 30.0f; // Selection radius in pixels

  if (mouseState.leftMousePressed && isSelected) {
    // Unproject current and last mouse to world using the depth from
    // screenPos.z
    glm::vec3 currMouse3D = glm::vec3(
        mouseState.pos.x, float(windowHeight) - mouseState.pos.y, screenPos.z);
    glm::vec3 lastMouse3D =
        glm::vec3(mouseState.lastPos.x,
                  float(windowHeight) - mouseState.lastPos.y, screenPos.z);

    glm::vec3 currWorld =
        glm::unProject(currMouse3D, view * model, projection, viewport);
    glm::vec3 lastWorld =
        glm::unProject(lastMouse3D, view * model, projection, viewport);

    glm::vec3 difference = currWorld - lastWorld;
    float diffLen = glm::length(difference);
    if (diffLen > elasticity && diffLen > 0.0f)
      difference = (elasticity / diffLen) * difference;
    prevPos = pos - difference;
  }

  if (mouseState.rightMousePressed && isSelected) {
    for (auto spring : attachedSprings) {
      if (spring)
        spring->Break();
    }
  }

  if (isPinned) {
    pos = initPos;
    return;
  }

  glm::vec3 effectiveAcc = mass * acceleration + force; // mass = 1

  glm::vec3 newPos = pos + (pos - prevPos) * (1.0f - drag) +
                     effectiveAcc * (1.0f - drag) * deltaTime * deltaTime;
  prevPos = pos;
  pos = newPos;
}
