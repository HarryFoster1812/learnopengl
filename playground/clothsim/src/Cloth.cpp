#include "Cloth.hpp"
#include <iostream>

Cloth::Cloth(float clothWidth, float clothHeight, int numPointsWidth,
             int numPointsHeight, float xOffset, float yOffset, float zOffset,
             ClothPlane plane)
    : numWidth(numPointsWidth), numHeight(numPointsHeight) {

  spacing = clothWidth / (numWidth - 1);                 // Horizontal spacing
  float verticalSpacing = clothHeight / (numHeight - 1); // Vertical spacing
  std::cout << "numWidth: " << numWidth << ", numHeight: " << numHeight
            << std::endl;
  std::cout << "numWidth * numHeight: " << numWidth * numHeight << std::endl;
  points.reserve(numWidth * numHeight);
  points.resize(numWidth * numHeight);
  std::cout << "After points.resize, vertexArray.size(): " << vertexArray.size()
            << std::endl;

  vertexArray.reserve(numWidth * numHeight);
  vertexArray.resize(numWidth * numHeight);
  std::cout << vertexArray.size() << std::endl;

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
      // if (y == 0)
      //   points[idx].pin(); // Pin top row

      vertexArray[idx].position = pos;
      // vertexArray[idx].normal = glm::vec3(0.0f, 0.0f, 1.0f);
      // vertexArray[idx].texCoords =
      //     glm::vec2(static_cast<float>(x) / (numWidth - 1),
      //               static_cast<float>(y) / (numHeight - 1));
      std::cout << "(" << vertexArray[idx].position.x << ", " // x
                << vertexArray[idx].position.y << ", "        // y
                << vertexArray[idx].position.z                // x
                << ")" << std::endl;
    }
  }

  // Create structural springs
  for (int y = 0; y < numHeight; ++y) {
    for (int x = 0; x < numWidth; ++x) {
      int idx = y * numWidth + x;
      if (x < numWidth - 1) {
        springs.emplace_back(&points[idx], &points[idx + 1], spacing, idx,
                             idx + 1);
        points[idx].addSpring(&springs.back());
        points[idx + 1].addSpring(&springs.back());
      }
      if (y < numHeight - 1) {
        springs.emplace_back(&points[idx], &points[idx + numWidth],
                             verticalSpacing, idx, idx + numWidth);
        points[idx].addSpring(&springs.back());
        points[idx + numWidth].addSpring(&springs.back());
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
  glBufferData(GL_ARRAY_BUFFER, vertexArray.capacity() * sizeof(Vertex),
               vertexArray.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               lineIndices.size() * sizeof(unsigned int), lineIndices.data(),
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void *)offsetof(Vertex, normal));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  //                       (void *)offsetof(Vertex, texCoords));
  // glEnableVertexAttribArray(2);

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
    // vertexArray[i].normal = glm::vec3(0.0f, 0.0f, 1.0f);
  }
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArray.size() * sizeof(Vertex),
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
  }

  updateVertices();
}

void Cloth::render() {
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, static_cast<GLsizei>(lineIndices.size()),
                 GL_UNSIGNED_INT, 0);

  glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertexArray.capacity()));
  glBindVertexArray(0);
}
