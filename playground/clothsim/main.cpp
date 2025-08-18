#include <cmath>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <glm/ext/vector_float2.hpp>
#include <iostream>

#include "./src/Cloth.hpp"
#include "./src/Mouse.hpp"
#include "./src/Plane.hpp"
#include <core/Camera.hpp>
#include <core/Shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <limits>

float fbWidth = 800.0f, fbHeight = 600.0f;

Camera camera(glm::vec3(0.0f, 20.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,
              0.0f);

float deltaTime = 0.0f;
float timeStep = 0.0166666666667f;
float lastFrame = 0.0f;

MouseState mouseState;

bool wasPausePressed = false; // outside your loop
bool isSimPaused = false;

bool cursorEnabled = false;

float drag = 0.10f;
float pointMass = 2.0f;
float prevPointMass = 2.0f;
glm::vec3 gravity{0.0f, -9.80665f, 0.0f};
float springK = 100.0f;
float elasticity = 0.1f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

void toggleCursor(GLFWwindow *window);

void processInput(GLFWwindow *window);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

GLFWwindow *setup();

void initIMGUI(GLFWwindow *window);

void drawIMGUI();

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  GLFWwindow *window = setup();

  initIMGUI(window);

  camera.setMovementSpeed(10.0f);

  glm::vec3 clothColour = glm::vec3(1.0f);
  glm::vec3 floorColour = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 rampColour = glm::vec3(0.5f, 0.5f, 0.0f);

  Shader clothShader("shaders/vertex.vert", "shaders/fragment.frag");
  clothShader.use();
  clothShader.setVec3("colour", clothColour);

  Shader floorShader("shaders/vertex.vert", "shaders/floorfragment.frag");
  floorShader.use();
  floorShader.setVec3("colour", floorColour);

  Shader rampShader("shaders/vertex.vert", "shaders/floorfragment.frag");
  rampShader.use();
  rampShader.setVec3("colour", rampColour);

  // 4m x 2m cloth, 100 points across, 50 points down
  float clothWidth = 50.0f;
  float clothHeight = 50.0f;
  float xOffset = 0.0f;
  float yOffset = 55.0f;
  float zOffset = -20.0f;

  Cloth cloth(clothWidth,                // physical width in world units
              clothHeight,               // physical height in world units
              20,                        // points horizontally
              20,                        // points vertically
              xOffset, yOffset, zOffset, // offsets in world space
              ClothPlane::XY);

  Plane floor(&floorShader, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
              glm::vec2(100, 100));

  cloth.addObject(&floor);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawIMGUI();

    float aspect = (fbHeight != 0.0f) ? (fbWidth / fbHeight) : 1.3333f;
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()), aspect, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    floorShader.use();

    floorShader.setMat4("projection", projection);
    floorShader.setMat4("view", view);
    floorShader.setMat4("model", model);
    floor.Draw();

    clothShader.use();
    clothShader.setMat4("projection", projection);
    clothShader.setMat4("view", view);
    clothShader.setMat4("model", model);
    clothShader.setVec2("u_resolution", glm::vec2(fbWidth, fbHeight));

    cloth.setMatrices(projection, view, model);
    if (prevPointMass != pointMass) {
      cloth.setPointMass(pointMass);
      prevPointMass = pointMass;
    }

    if (!isSimPaused)
      cloth.run(timeStep, drag, gravity, springK, elasticity, mouseState,
                static_cast<int>(fbWidth), static_cast<int>(fbHeight));
    cloth.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
  fbWidth = static_cast<float>(width);
  fbHeight = static_cast<float>(height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  if (cursorEnabled)
    return;

  if (mouseState.firstMouse) {
    mouseState.lastPos = {static_cast<float>(xpos), static_cast<float>(ypos)};
    mouseState.firstMouse = false;
    return;
  }

  float xoffset = static_cast<float>(xpos) - mouseState.lastPos.x;
  float yoffset = mouseState.lastPos.y - static_cast<float>(ypos);

  mouseState.lastPos = {static_cast<float>(xpos), static_cast<float>(ypos)};

  camera.processMouseMovement(xoffset, yoffset);

  // Keep logical cursor in screen center for picking
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  mouseState.pos = {width / 2.0f, height / 2.0f};
}

void toggleCursor(GLFWwindow *window) {
  cursorEnabled = !cursorEnabled;
  glfwSetInputMode(window, GLFW_CURSOR,
                   cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
  mouseState.firstMouse =
      true; // Prevent camera jump after enabling cursor again
}

void processInput(GLFWwindow *window) {

  static bool tabPressed = false;

  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
    toggleCursor(window);
    tabPressed = true;
  }
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
    tabPressed = false;
  }

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

  bool isPPressed = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);

  if (isPPressed && !wasPausePressed) {
    isSimPaused = !isSimPaused; // toggle only once per key press
  }

  wasPausePressed = isPPressed; // update previous state

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

void initIMGUI(GLFWwindow *window) {

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install
                     // GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
}
void drawIMGUI() {

  ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Simulation Settings"); // Pass a pointer to our bool variable
  ImGui::SliderFloat("Mass", &pointMass, 0.0f,
                     10.0f); // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::SliderFloat("Drag", &drag, 0.0f,
                     1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::SliderFloat3("Gravity", (float *)&gravity, -10.0f,
                      10.0f); // Edit 3 floats representing a color
  //
  ImGui::SliderFloat("SpringK", &springK, 0.0f,
                     1000.0f); // Edit 1 float using a slider from
                               // 0.0f to 1.0f
  //
  ImGui::SliderFloat("Elasticity", &elasticity, -2.0f,
                     2.0f); // Edit 1 float using a slider from
                            // 0.0f to 1.0f

  ImGui::Checkbox("PauseSim", &isSimPaused);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
