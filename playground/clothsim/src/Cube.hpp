#pragma once
#include "CollidableObject.hpp"
#include <algorithm>
#include <core/Shader.hpp>

class Cube : public CollidableObject {
public:
  glm::vec3 size;
  glm::vec3 vertices[8];
  unsigned int indices[36] = {// front
                              0, 1, 2, 2, 3, 0,
                              // right
                              1, 5, 6, 6, 2, 1,
                              // back
                              7, 6, 5, 5, 4, 7,
                              // left
                              4, 0, 3, 3, 7, 4,
                              // bottom
                              4, 5, 1, 1, 0, 4,
                              // top
                              3, 2, 6, 6, 7, 3};
  unsigned int VBO, VAO, EBO;
  Shader *shader;

  Cube(const glm::vec3 &center, const glm::vec3 &cubeSize, Shader *shader)
      : CollidableObject(center), size(cubeSize), shader(shader) {
    glm::vec3 half = size * 0.5f;

    vertices[0] = pos + glm::vec3(-half.x, -half.y, half.z);
    vertices[1] = pos + glm::vec3(half.x, -half.y, half.z);
    vertices[2] = pos + glm::vec3(half.x, half.y, half.z);
    vertices[3] = pos + glm::vec3(-half.x, half.y, half.z);

    vertices[4] = pos + glm::vec3(-half.x, -half.y, -half.z);
    vertices[5] = pos + glm::vec3(half.x, -half.y, -half.z);
    vertices[6] = pos + glm::vec3(half.x, half.y, -half.z);
    vertices[7] = pos + glm::vec3(-half.x, half.y, -half.z);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
  }

  void Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
