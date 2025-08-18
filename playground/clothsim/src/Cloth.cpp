#include "Cloth.hpp"
#include <iostream>

int countSprings(int numWidth, int numHeight) {
  int structural = (numWidth - 1) * numHeight + (numHeight - 1) * numWidth;
  int shearing = 2 * (numWidth - 1) * (numHeight - 1);
  int bending = (numWidth - 2) * numHeight + (numHeight - 2) * numWidth;

  return structural + shearing + bending;
}

Cloth::Cloth(float clothWidth, float clothHeight, int numPointsWidth,
             int numPointsHeight, float xOffset, float yOffset, float zOffset,
             ClothPlane plane)
    : numWidth(numPointsWidth), numHeight(numPointsHeight) {

  spacing = clothWidth / (numWidth - 1);                 // Horizontal spacing
  float verticalSpacing = clothHeight / (numHeight - 1); // Vertical spacing
  points.reserve(numWidth * numHeight);
  points.resize(numWidth * numHeight);

  vertexArray.reserve(numWidth * numHeight);
  vertexArray.resize(numWidth * numHeight);

  springs.reserve(countSprings(
      numWidth, numHeight)); // reserve so that the pointers are not invalidated

  for (int y = 0; y < numHeight; ++y) {
    for (int x = 0; x < numWidth; ++x) {
      int idx = y * numWidth + x;

      glm::vec3 pos;
      switch (plane) {
      case ClothPlane::XY:
        pos = glm::vec3(x * spacing + xOffset, -y * verticalSpacing + yOffset,
                        zOffset);
        break;
      case ClothPlane::XZ:
        pos = glm::vec3(x * spacing + xOffset, yOffset,
                        y * verticalSpacing + zOffset);
        break;
      case ClothPlane::YZ:
        pos = glm::vec3(xOffset, x * spacing + yOffset,
                        y * verticalSpacing + zOffset);

        break;
      }

      points[idx] = Point(pos.x, pos.y, pos.z);

      if (plane == ClothPlane::XY && y == 0)
        points[idx].pin(); // Pin top row

      if (plane == ClothPlane::XZ) {
        bool isTopLeftCorner = (x == 0 && y == 0);
        bool isTopRightCorner = (x == numWidth - 1 && y == 0);
        bool isBottomLeftCorner = (x == 0 && y == numHeight - 1);
        bool isBottomRightCorner = (x == numWidth - 1 && y == numHeight - 1);

        if (isTopLeftCorner || isTopRightCorner || isBottomLeftCorner ||
            isBottomRightCorner) {
          points[idx].pin();
        }
      }

      vertexArray[idx].position = pos;
      vertexArray[idx].normal = glm::vec3(0.0f, 0.0f, 1.0f);
      vertexArray[idx].texCoords =
          glm::vec2(static_cast<float>(x) / (numWidth - 1),
                    static_cast<float>(y) / (numHeight - 1));
    }
  }

  // Structural springs
  for (int y = 0; y < numHeight; ++y) {
    for (int x = 0; x < numWidth; ++x) {
      int idx = y * numWidth + x;

      if (x < numWidth - 1) { // right neighbor
        springs.emplace_back(&points[idx], &points[idx + 1], spacing, idx,
                             idx + 1);
        points[idx].addSpring(&springs.back());
        points[idx + 1].addSpring(&springs.back());
      }
      if (y < numHeight - 1) { // bottom neighbor
        springs.emplace_back(&points[idx], &points[idx + numWidth],
                             verticalSpacing, idx, idx + numWidth);
        points[idx].addSpring(&springs.back());
        points[idx + numWidth].addSpring(&springs.back());
      }
    }
  }

  // Shearing springs (diagonals)
  for (int y = 0; y < numHeight - 1; ++y) {
    for (int x = 0; x < numWidth - 1; ++x) {
      int idx = y * numWidth + x;

      // bottom-right diagonal
      springs.emplace_back(
          &points[idx], &points[idx + numWidth + 1],
          std::sqrt(spacing * spacing + verticalSpacing * verticalSpacing), idx,
          idx + numWidth + 1);
      points[idx].addSpring(&springs.back());
      points[idx + numWidth + 1].addSpring(&springs.back());

      // bottom-left diagonal
      springs.emplace_back(
          &points[idx + 1], &points[idx + numWidth],
          std::sqrt(spacing * spacing + verticalSpacing * verticalSpacing),
          idx + 1, idx + numWidth);
      points[idx + 1].addSpring(&springs.back());
      points[idx + numWidth].addSpring(&springs.back());
    }
  }

  // Bending springs (skip one point)
  for (int y = 0; y < numHeight; ++y) {
    for (int x = 0; x < numWidth; ++x) {
      int idx = y * numWidth + x;

      if (x < numWidth - 2) { // 2 to the right
        springs.emplace_back(&points[idx], &points[idx + 2], spacing * 2, idx,
                             idx + 2);
        points[idx].addSpring(&springs.back());
        points[idx + 2].addSpring(&springs.back());
      }
      if (y < numHeight - 2) { // 2 below
        springs.emplace_back(&points[idx], &points[idx + 2 * numWidth],
                             verticalSpacing * 2, idx, idx + 2 * numWidth);
        points[idx].addSpring(&springs.back());
        points[idx + 2 * numWidth].addSpring(&springs.back());
      }
    }
  }

  // Line indices for springs
  for (const auto &spring : springs) {
    if (spring.isActive()) {
      lineIndices.push_back(static_cast<unsigned int>(spring.getP1Index()));
      lineIndices.push_back(static_cast<unsigned int>(spring.getP2Index()));
    }
  }

  // Setup OpenGL buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexArray.capacity() * sizeof(RenderVertex),
               vertexArray.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               lineIndices.size() * sizeof(unsigned int), lineIndices.data(),
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertex),
                        (void *)offsetof(RenderVertex, normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex),
                        (void *)offsetof(RenderVertex, texCoords));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

Cloth::~Cloth() {
  if (VAO)
    glDeleteVertexArrays(1, &VAO);
  if (VBO)
    glDeleteBuffers(1, &VBO);
  if (EBO)
    glDeleteBuffers(1, &EBO);
}

void Cloth::setMatrices(const glm::mat4 &proj, const glm::mat4 &view,
                        const glm::mat4 &model) {
  projectionMatrix = proj;
  viewMatrix = view;
  modelMatrix = model;
}

void Cloth::updateVertices() {
  for (size_t i = 0; i < points.size(); ++i) {
    vertexArray[i].position = points[i].GetPosition();
    // Optional: compute normal (skipped here)
    vertexArray[i].normal = glm::vec3(0.0f, 0.0f, 1.0f);
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0,
                  vertexArray.capacity() * sizeof(RenderVertex),
                  vertexArray.data());

  // Update line indices if springs broken

  lineIndices.clear();
  for (const auto &spring : springs) {
    if (spring.isActive()) {
      lineIndices.push_back(static_cast<unsigned int>(spring.getP1Index()));
      lineIndices.push_back(static_cast<unsigned int>(spring.getP2Index()));
    }
  }
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               lineIndices.size() * sizeof(unsigned int), lineIndices.data(),
               GL_DYNAMIC_DRAW);
  glBindVertexArray(0);
}

void Cloth::run(float deltaTime, float drag, const glm::vec3 &gravity,
                float springK, float elasticity, MouseState &mouseState,
                int windowWidth, int windowHeight) {
  // Reset forces
  for (auto &point : points) {
    point.resetForce();
  }

  // Apply spring forces
  for (auto &spring : springs) {
    if (!spring.isActive())
      continue;
    Point *p1 = spring.getP1();
    Point *p2 = spring.getP2();
    glm::vec3 dir = p1->GetPosition() - p2->GetPosition();
    float dist = glm::length(dir);
    if (dist > 0.0f) {
      glm::vec3 force =
          -springK * (dist - spring.getRestLength()) * (dir / dist);
      p1->addForce(force);
      p2->addForce(-force);
    }
  }

  // Update points
  for (auto &point : points) {
    point.update(deltaTime, drag, gravity, elasticity, mouseState, windowWidth,
                 windowHeight, projectionMatrix, viewMatrix, modelMatrix);
    point.resolveCollisions(planeColliders);
  }

  updateVertices();
}

void Cloth::render() {
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, static_cast<GLsizei>(lineIndices.size()),
                 GL_UNSIGNED_INT, 0);

  // glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertexArray.capacity()));
  glBindVertexArray(0);
}
