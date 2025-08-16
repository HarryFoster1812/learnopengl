#pragma once
#include "Plane.hpp"
#include <glm/glm.hpp>
#include <vector>

struct MouseState;
class Spring;

class Point {
private:
  std::vector<Spring *> attachedSprings;

  glm::vec3 pos{};
  glm::vec3 prevPos{};
  glm::vec3 initPos{};
  glm::vec3 force = glm::vec3(0.0f);
  bool isPinned = false;
  bool isSelected = false;
  float mass = 1.0f;

  void KeepInsideView(int width, int height);

public:
  Point() = default;
  Point(float x, float y, float z);
  explicit Point(glm::vec3 position);
  ~Point() = default;

  void addSpring(Spring *spring);

  const glm::vec3 &GetPosition() const { return pos; }
  void setPosition(float x, float y, float z);

  void pin();

  void addForce(const glm::vec3 &f) { force += f; }
  void resetForce() { force = glm::vec3(0.0f); }

  void update(float deltaTime, float drag, const glm::vec3 &acceleration,
              float elasticity, MouseState &mouseState, int windowWidth,
              int windowHeight, const glm::mat4 &projection,
              const glm::mat4 &view, const glm::mat4 &model);
  void setMass(const float newMass) { mass = newMass; }

  void resolveCollisions(const std::vector<Plane *> &planes);
};
