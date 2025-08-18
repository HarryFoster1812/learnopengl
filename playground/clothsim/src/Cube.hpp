#pragma once
#include "CollidableObject.hpp"
#include <algorithm>

class Cube : public CollidableObject {
public:
  glm::vec3 size;
  glm::vec3 vertices[8];

  Cube(const glm::vec3 &center, const glm::vec3 &cubeSize)
      : CollidableObject(center), size(cubeSize) {
    glm::vec3 half = size * 0.5f;

    vertices[0] = pos + glm::vec3(-half.x, -half.y, half.z);
    vertices[1] = pos + glm::vec3(half.x, -half.y, half.z);
    vertices[2] = pos + glm::vec3(half.x, half.y, half.z);
    vertices[3] = pos + glm::vec3(-half.x, half.y, half.z);

    vertices[4] = pos + glm::vec3(-half.x, -half.y, -half.z);
    vertices[5] = pos + glm::vec3(half.x, -half.y, -half.z);
    vertices[6] = pos + glm::vec3(half.x, half.y, -half.z);
    vertices[7] = pos + glm::vec3(-half.x, half.y, -half.z);
  }

  void resolveCollision(glm::vec3 &pointPos,
                        glm::vec3 &prevPos) const override {
    glm::vec3 half = size * 0.5f;
    glm::vec3 min = pos - half;
    glm::vec3 max = pos + half;

    if (pointPos.x >= min.x && pointPos.x <= max.x && pointPos.y >= min.y &&
        pointPos.y <= max.y && pointPos.z >= min.z && pointPos.z <= max.z) {

      float dx1 = max.x - pointPos.x;
      float dx2 = pointPos.x - min.x;
      float dy1 = max.y - pointPos.y;
      float dy2 = pointPos.y - min.y;
      float dz1 = max.z - pointPos.z;
      float dz2 = pointPos.z - min.z;

      float minPen = std::min({dx1, dx2, dy1, dy2, dz1, dz2});
      glm::vec3 correction(0.0f);

      if (minPen == dx1)
        correction.x = dx1;
      else if (minPen == dx2)
        correction.x = -dx2;
      else if (minPen == dy1)
        correction.y = dy1;
      else if (minPen == dy2)
        correction.y = -dy2;
      else if (minPen == dz1)
        correction.z = dz1;
      else if (minPen == dz2)
        correction.z = -dz2;

      const float epsilon = 0.001f;
      pointPos += correction + glm::vec3(epsilon);
      prevPos += correction * 0.5f;
    }
  }
};
