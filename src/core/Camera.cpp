#include <core/Camera.hpp>

// Constructor(s)
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position), Up(up), Yaw(yaw), Pitch(pitch) {
  updateCameraVectors(); // calculate the front and right vector
}

// Get the view matrix
glm::mat4 Camera::getViewMatrix() const {

  return glm::lookAt(Position, Position + Front, Up);
}

// Process input
void Camera::processKeyboard(CameraMovement direction, float deltaTime) {

  float cameraSpeed = MovementSpeed * deltaTime;

  switch (direction) {
  case CameraMovement::FORWARD:
    Position += cameraSpeed * Front;
    break;
  case CameraMovement::BACKWARD:
    Position -= cameraSpeed * Front;
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
}
void Camera::processMouseMovement(float xoffset, float yoffset,
                                  bool constrainPitch) {

  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;
  Yaw += xoffset;
  Pitch += yoffset;

  if (constrainPitch) {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  updateCameraVectors();
}
void Camera::processMouseScroll(float yoffset) {

  Zoom -= (float)yoffset;
  if (Zoom < 1.0f)
    Zoom = 1.0f;
  if (Zoom > 90.0f)
    Zoom = 90.0f;
}

// Setters
void Camera::setPosition(const glm::vec3 &position) { Position = position; }
void Camera::setYaw(float yaw) { Yaw = yaw; }
void Camera::setPitch(float pitch) { Pitch = pitch; }
void Camera::setZoom(float zoom) { Zoom = zoom; }

// Getters
glm::vec3 Camera::getPosition() const { return Position; }
float Camera::getYaw() const { return Yaw; }
float Camera::getPitch() const { return Pitch; }
float Camera::getZoom() const { return Zoom; }
glm::vec3 Camera::getFront() const { return Front; }

void Camera::updateCameraVectors() {
  Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front.y = sin(glm::radians(Pitch));
  Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(Front);
  Right = glm::normalize(glm::cross(Front, WorldUp));
}
