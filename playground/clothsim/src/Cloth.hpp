#pragma once
#include "CollidableObject.hpp"
#include "Mouse.hpp"
#include "Point.hpp"
#include "Spring.hpp"
#include <cstddef>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct RenderVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

enum class ClothPlane {
  XY, // X horizontal, Y vertical
  XZ, // X horizontal, Z vertical
  YZ  // Y horizontal, Z vertical
};

class Cloth {
private:
  int numWidth;
  int numHeight;
  float spacing;
  std::vector<Point> points;
  std::vector<Spring> springs;
  std::vector<RenderVertex> vertexArray;
  std::vector<unsigned int> lineIndices;
  std::vector<unsigned int> triIndices;
  std::vector<CollidableObject *> objectColliders;

  unsigned int VAO = 0, VBO = 0, EBO_lines = 0, EBO_tri = 0;

  glm::mat4 modelMatrix{1.0f};
  glm::mat4 viewMatrix{1.0f};
  glm::mat4 projectionMatrix{1.0f};

  void updateVertices();

public:
  Cloth(float clothWidth, float clothHeight, int numPointsWidth,
        int numPointsHeight, float xOffset, float yOffset, float zOffset,
        ClothPlane plane);
  ~Cloth();

  void setMatrices(const glm::mat4 &proj, const glm::mat4 &view,
                   const glm::mat4 &model);

  void run(float deltaTime, float drag, const glm::vec3 &gravity, float springK,
           float elasticity, MouseState &mouseState, int windowWidth,
           int windowHeight);

  void setPointMass(float newMass) {
    for (auto &point : points) {
      point.setMass(newMass);
    }
  }

  void addObject(CollidableObject *object) {
    objectColliders.push_back(object);
  }

  void render(bool wireframe = false);
};
