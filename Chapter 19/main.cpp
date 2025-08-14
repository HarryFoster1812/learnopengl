#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <core/Camera.hpp>
#include <core/Model.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

float fbWidth = 800.0f, fbHeight = 600.0f;
double lastX = 400, lastY = 300;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
Camera camera(cameraPos, cameraUp, yaw, pitch);

bool firstMouse = true;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// call back function which resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  fbWidth = width;
  fbHeight = height;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  int winWidth, winHeight;
  glfwGetWindowSize(window, &winWidth, &winHeight);

  float centerX = (float)winWidth / 2;
  float centerY = (float)winHeight / 2;

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
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
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(yoffset);
}

int main(int argc, char *argv[]) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  // catch failure
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // loads and changes the opengl settings for the target OS when compiling
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  Shader lightingShader("vertex.vert", "fragment.frag");

  glViewport(0, 0, 800, 600);

  // setting the resize callback

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, cursor_position_callback);

  glfwSetScrollCallback(window, scroll_callback);

  // define the triangle vertices

  Model earthModel("../models/backpack/backpack.obj");

  glEnable(GL_DEPTH_TEST);
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader.use();
    // view/projection transformations
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.getZoom()),
                         (float)fbWidth / (float)fbHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    lightingShader.setVec3("viewPos", camera.getPosition());
    lightingShader.setFloat("shininess", 32.0f);

    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    lightingShader.setVec3("spotLight.position", camera.getPosition());
    lightingShader.setVec3("spotLight.direction", camera.getFront());
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff",
                            glm::cos(glm::radians(15.0f)));

    // world transformation

    glm::mat4 model = glm::mat4(1.0f);
    model =
        glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // adjust position
    model = glm::scale(model, glm::vec3(0.2f));               // scale down
    lightingShader.setMat4("model", model);

    earthModel.Draw(lightingShader);

    // again, a lamp object is weird when we only have a spot light, don't
    // render the light object
    // for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); ++i)
    // {
    //   lightCubeShader.use();
    //   lightCubeShader.setMat4("projection", projection);
    //   lightCubeShader.setMat4("view", view);
    //   model = glm::mat4(1.0f);
    //   model = glm::translate(model, pointLightPositions[i]);
    //   model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    //   lightCubeShader.setMat4("model", model);
    //
    //   glDrawArrays(GL_TRIANGLES, 0, 36);
    // }

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}
