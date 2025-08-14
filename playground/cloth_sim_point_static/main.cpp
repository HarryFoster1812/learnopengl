#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include <core/Camera.hpp>
#include <core/Shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct MouseState {
  bool firstMouse = true;
  glm::vec2 lastPos = {400.0f, 300.0f};
  glm::vec2 pos = {400.0f, 300.0f};
  bool leftMousePressed = false;
  bool rightMousePressed = false;
};

float fbWidth = 800.0f, fbHeight = 600.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,
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

  GLFWwindow *window = setup();

  Shader shader("vertex1.vert", "fragment.frag");

  glm::vec3 testPoints[30] = {
      {0.0f, 0.5f, -0.5f},        {-0.25f, 0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},       {0.0f, 0.5f, -0.444444f},
      {-0.25f, 0.5f, -0.444444f}, {-0.5f, 0.5f, -0.444444f},
      {0.0f, 0.5f, -0.388889f},   {-0.25f, 0.5f, -0.388889f},
      {-0.5f, 0.5f, -0.388889f},  {0.0f, 0.5f, -0.333333f},
      {-0.25f, 0.5f, -0.333333f}, {-0.5f, 0.5f, -0.333333f},
      {0.0f, 0.5f, -0.277778f},   {-0.25f, 0.5f, -0.277778f},
      {-0.5f, 0.5f, -0.277778f},  {0.0f, 0.5f, -0.222222f},
      {-0.25f, 0.5f, -0.222222f}, {-0.5f, 0.5f, -0.222222f},
      {0.0f, 0.5f, -0.166667f},   {-0.25f, 0.5f, -0.166667f},
      {-0.5f, 0.5f, -0.166667f},  {0.0f, 0.5f, -0.111111f},
      {-0.25f, 0.5f, -0.111111f}, {-0.5f, 0.5f, -0.111111f},
      {0.0f, 0.5f, -0.0555556f},  {-0.25f, 0.5f, -0.0555556f},
      {-0.5f, 0.5f, -0.0555556f}, {0.0f, 0.5f, 0.0f},
      {-0.25f, 0.5f, 0.0f},       {-0.5f, 0.5f, 0.0f}};

  // define the triangle vertices
  // create a buffer that will be stored on the GPU
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // bind GL_ARRAY_BUFFER to point to the VBO buffer
  glBindVertexArray(VAO);
  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(testPoints), testPoints, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // setting the texture wrapping

  double xpos, ypos;
  // basic render loop
  while (!glfwWindowShouldClose(window)) {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Convert cursor position from window coords to framebuffer coords if
    // needed:

    shader.use();

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    shader.use();

    float aspect = (fbHeight != 0.0f) ? (fbWidth / fbHeight) : 1.3333f;
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()), aspect, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", model);

    // bind textures on corresponding texture units

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 30);
    glBindVertexArray(0);

    // draw call
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // close and clean any memory used
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}
