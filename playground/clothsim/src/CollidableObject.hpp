#pragma once
#include <glm/glm.hpp>

// Base class for all collidable objects
class CollidableObject {
public:
  glm::vec3 pos;

  CollidableObject(const glm::vec3 &position) : pos(position) {}
  virtual ~CollidableObject() = default;

  // Pure virtual collision resolver
  virtual void resolveCollision(glm::vec3 &pointPos,
                                glm::vec3 &prevPos) const = 0;
};
