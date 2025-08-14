#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include "./src/Cloth.hpp"
#include "./src/Floor.hpp"
#include "./src/Mouse.hpp"
#include <core/Camera.hpp>
#include <core/Shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float fbWidth = 800.0f, fbHeight = 600.0f;

Camera camera(glm::vec3(0.0f, 20.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,
              0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

MouseState mouseState;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
  fbWidth = static_cast<float>(width);
  fbHeight = static_cast<float>(height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;
  if (mouseState.firstMouse) {
    mouseState.lastPos.x = static_cast<float>(xpos);
    mouseState.lastPos.y = static_cast<float>(ypos);
    mouseState.firstMouse = false;
  }

  float xoffset = static_cast<float>(xpos) - mouseState.lastPos.x;
  float yoffset = mouseState.lastPos.y - static_cast<float>(ypos);

  mouseState.lastPos = mouseState.pos;
  mouseState.pos = {static_cast<float>(xpos), static_cast<float>(ypos)};

  camera.processMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.processKeyboard(CameraMovement::DOWN, deltaTime);

  mouseState.leftMousePressed =
      (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
  mouseState.rightMousePressed =
      (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;
  camera.processMouseScroll(static_cast<float>(yoffset));
}

GLFWwindow *setup() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Cloth Simulator", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glEnable(GL_DEPTH_TEST);
  glPointSize(5.0f); // or higher

  return window;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  GLFWwindow *window = setup();
  camera.setMovementSpeed(10.0f);

  glm::vec3 clothColour = glm::vec3(1.0f);
  glm::vec3 floorColour = glm::vec3(1.0f, 0.0f, 0.0f);

  Shader clothShader("shaders/vertex.vert", "shaders/fragment.frag");
  clothShader.use();
  clothShader.setVec3("colour", clothColour);

  Shader floorShader("shaders/vertex.vert", "shaders/fragment.frag");
  floorShader.use();
  floorShader.setVec3("colour", floorColour);

  // 4m x 2m cloth, 100 points across, 50 points down
  float clothWidth = 50.0f;
  float clothHeight = 50.0f;
  float xOffset = -25.0f;
  float yOffset = 20.0f;
  float zOffset = -50.0f;

  Cloth cloth(clothWidth,                // physical width in world units
              clothHeight,               // physical height in world units
              60,                        // points horizontally
              60,                        // points vertically
              xOffset, yOffset, zOffset, // offsets in world space
              ClothPlane::XZ);

  Floor floor(&floorShader, clothWidth, clothHeight,
              glm::vec3(xOffset, yOffset, zOffset));

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    clothShader.use();

    float aspect = (fbHeight != 0.0f) ? (fbWidth / fbHeight) : 1.3333f;
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()), aspect, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    clothShader.setMat4("projection", projection);
    clothShader.setMat4("view", view);
    clothShader.setMat4("model", model);

    cloth.setMatrices(projection, view, model);
    cloth.run(deltaTime, 0.01f, glm::vec3(0.0f, -1.0f, 0.0f), 500.0f, 0.1f,
              mouseState, static_cast<int>(fbWidth),
              static_cast<int>(fbHeight));
    cloth.render();

    floorShader.use();

    floorShader.setMat4("projection", projection);
    floorShader.setMat4("view", view);
    floorShader.setMat4("model", model);
    floor.Draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
