#include <core/Shader.hpp>
#include <glm/ext/vector_float3.hpp>

struct Floor {
  unsigned int VAO, VBO, EBO;
  Shader *shader; // pointer to your shader

  glm::vec3 vertices[4];
  unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
  float yPos = 0.0f;

  // Constructor
  Floor(Shader *shaderPtr, float clothWidth, float clothHeight,
        glm::vec3 clothOffset)
      : shader(shaderPtr) {
    float margin = 10.0f;

    float floorXMin = clothOffset.x - margin;
    float floorXMax = clothOffset.x + clothWidth + margin;
    float floorZMin = clothOffset.z - margin;
    float floorZMax = clothOffset.z + clothHeight + margin;

    vertices[0] = {floorXMin, yPos, floorZMin};
    vertices[1] = {floorXMax, yPos, floorZMin};
    vertices[2] = {floorXMax, yPos, floorZMax};
    vertices[3] = {floorXMin, yPos, floorZMax};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
  }

  void Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};
