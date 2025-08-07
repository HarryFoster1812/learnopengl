#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
public:
  // Constructor(s)
  Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

  // Get the view matrix
  glm::mat4 getViewMatrix() const;

  // Process input
  virtual void processKeyboard(CameraMovement direction, float deltaTime);
  virtual void processMouseMovement(float xoffset, float yoffset,
                                    bool constrainPitch = true);
  virtual void processMouseScroll(float yoffset);

  // Setters
  void setPosition(const glm::vec3 &position);
  void setYaw(float yaw);
  void setPitch(float pitch);
  void setZoom(float zoom);

  // Getters
  glm::vec3 getPosition() const;
  float getYaw() const;
  float getPitch() const;
  float getZoom() const;
  glm::vec3 getFront() const;

protected:
  // Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

  // Euler Angles
  float Yaw;
  float Pitch;

  // Camera options
  float MovementSpeed = 2.5f;
  float MouseSensitivity = 0.1f;
  float Zoom = 1.0f;

  // Updates the camera vectors based on the updated Euler angles
  void updateCameraVectors();
};
