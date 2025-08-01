#include "../../glad/glad.h"
#include "../../include/Shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

int fbWidth, fbHeight;
double mouseX = 0, mouseY = 0;

// call back function which resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  fbWidth = width;
  fbHeight = height;
  Shader *shader = static_cast<Shader *>(glfwGetWindowUserPointer(window));
  if (shader)
    shader->setV2("window_res", width, height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  int winWidth, winHeight;
  glfwGetWindowSize(window, &winWidth, &winHeight);

  float scaleX = (float)fbWidth / (float)winWidth;
  float scaleY = (float)fbHeight / (float)winHeight;

  mouseX = xpos * scaleX;
  mouseY = fbHeight - ypos * scaleY; // flip y
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

  Shader shader("vertex.vert", "fragment.frag");

  glViewport(0, 0, 800, 600);

  // setting the resize callback

  glfwSetWindowUserPointer(window, &shader);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetCursorPosCallback(window, cursor_position_callback);

  // define the triangle vertices
  float vertices[] = {-1.0f, 1.0f,  0.0f,  // top left
                      -1.0f, -1.0f, 0.0f,  // bottom left
                      1.0f,  -1.0f, 0.0f,  // bottom right
                      1.0f,  1.0f,  0.0f}; // top right
  unsigned int indices[] = {
      0, 1, 2, // first triangle
      0, 2, 3  // second triangle
  };
  // create a buffer that will be stored on the GPU
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // bind GL_ARRAY_BUFFER to point to the VBO buffer
  glBindVertexArray(VAO);
  // 2. copy our vertices array in a vertex buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 3. copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  // 4. then set the vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  double xpos, ypos;
  // basic render loop
  while (!glfwWindowShouldClose(window)) {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Convert cursor position from window coords to framebuffer coords if
    // needed:

    shader.setV2("mouse_pos", mouseX, mouseY);
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // draw call
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // close and clean any memory used
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}
