#include <core/Camera.hpp>

class FPSCamera : public Camera {
public:
  FPSCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
      : Camera(position, up, yaw, pitch) {}

  void processKeyboard(CameraMovement direction, float deltaTime) override {
    // Lock Y position for ground-level movement

    float cameraSpeed = MovementSpeed * deltaTime;
    glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
    switch (direction) {
    case CameraMovement::FORWARD:
      Position += cameraSpeed * flatFront;
      break;
    case CameraMovement::BACKWARD:
      Position -= cameraSpeed * flatFront;
      break;
    case CameraMovement::LEFT:
      Position -= Right * cameraSpeed;
      break;
    case CameraMovement::RIGHT:
      Position += Right * cameraSpeed;
      break;
    case CameraMovement::UP:
      Position.y += cameraSpeed;
      break;
    case CameraMovement::DOWN:
      Position.y -= cameraSpeed;
      break;
    default:
      break;
    }
    Position.y = 0.0f;
  }

  void processMouseMovement(float xoffset, float yoffset,
                            bool constrainPitch = true) override {
    // Clamp pitch to prevent flipping
    Camera::processMouseMovement(xoffset, yoffset, true);
  }
};
