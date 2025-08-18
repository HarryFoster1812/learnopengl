#pragma once
#include "CollidableObject.hpp"
#include <core/Shader.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Plane : public CollidableObject {
public:
  unsigned int VAO, VBO, EBO;
  Shader *shader;

  glm::vec3 vertices[4];
  unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec3 bitangent;

  // Constructor
  Plane(Shader *shaderPtr, const glm::vec3 &center, const glm::vec3 &normal,
        const glm::vec2 &size)
      : shader(shaderPtr), CollidableObject(center), normal(normal) {

    // Compute tangent and bitangent safely
    glm::vec3 n = glm::normalize(normal);
    glm::vec3 up =
        (fabs(n.y) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    tangent = glm::normalize(glm::cross(up, n));
    bitangent = glm::normalize(glm::cross(n, tangent));

    glm::vec3 halfTangent = tangent * (size.x * 0.5f);
    glm::vec3 halfBitangent = bitangent * (size.y * 0.5f);

    // Compute four corner vertices
    vertices[0] = center - halfTangent - halfBitangent;
    vertices[1] = center + halfTangent - halfBitangent;
    vertices[2] = center + halfTangent + halfBitangent;
    vertices[3] = center - halfTangent + halfBitangent;

    // OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
  }

  void Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void resolveCollision(glm::vec3 &pointPos,
                        glm::vec3 &prevPos) const override {
    const float epsilon =
        0.01f; // how far from the plane we allow before correction
    const float stiffness = 0.5f; // fraction of correction to apply

    // Vector from plane center to point
    glm::vec3 toPoint = pointPos - pos;

    // Distance along plane normal (signed)
    float distance = glm::dot(toPoint, normal);

    // Only apply if point is very close or penetrating
    if (distance < epsilon) {
      // Convert to plane local space
      float localX = glm::dot(toPoint, tangent);
      float localY = glm::dot(toPoint, bitangent);

      // Half sizes
      float halfX = glm::length(vertices[1] - vertices[0]) * 0.5f;
      float halfY = glm::length(vertices[3] - vertices[0]) * 0.5f;

      // Only correct if within or near the rectangle bounds
      if (localX >= -halfX - epsilon && localX <= halfX + epsilon &&
          localY >= -halfY - epsilon && localY <= halfY + epsilon) {

        // Project point onto plane (soft correction)
        glm::vec3 correction = (epsilon - distance) * normal * stiffness;
        pointPos += correction;

        // Apply small fraction to previous position to reduce snapping
        prevPos += correction * 0.5f;
      }
    }
  }
};
