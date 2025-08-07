#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <core/Shader.hpp>
#include <iostream>
#include <stb/stb_image.h>

int fbWidth, fbHeight;
double mouseX = 0, mouseY = 0;

// call back function which resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  fbWidth = width;
  fbHeight = height;
  Shader *shader = static_cast<Shader *>(glfwGetWindowUserPointer(window));
  if (shader)
    shader->setVec2("window_res", width, height);
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
  float vertices[] = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // setting the texture wrapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  // setting the texture filtering nearest produces a definate pixel color
  // linear mixes all of the other colors together
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // after creating a texure we can use: to create the mipmaps for scaling
  // glGenerateMipmaps

  // load texture image
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("../textures/container.jpg", &width, &height, &nrChannels, 0);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  stbi_image_free(data);

  int width1, height1, nrChannels1;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data1 = stbi_load("../textures/awesomeface.png", &width1,
                                   &height1, &nrChannels1, 0);

  unsigned int texture1;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  if (data1) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data1);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load second texture" << std::endl;
  }

  shader.use();
  shader.setInt("texture0", 0); // or with shader class
  shader.setInt("texture1", 1); // or with shader class

  double xpos, ypos;
  // basic render loop
  while (!glfwWindowShouldClose(window)) {

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Convert cursor position from window coords to framebuffer coords if
    // needed:

    shader.use();
    shader.setVec2("mouse_pos", mouseX, mouseY);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

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
