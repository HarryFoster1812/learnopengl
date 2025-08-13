#include <algorithm>
#include <core/Mesh.hpp>
#include <vector>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<MaterialTexture> textures)
    : vertices(vertices), indices(indices), textures(std::move(textures)) {
  // now that we have all the required data, set the vertex buffers and its
  // attribute pointers.
  setupMesh();
}

// render the mesh
void Mesh::Draw(Shader &shader) {
  shader.use();

  // Set diffuse count and weights (example: equal weights)
  int diffuseCount = 0;
  float diffuseWeights[8] = {0};

  unsigned int diffuseNr = 1;
  for (unsigned int i = 0; i < textures.size(); i++) {
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNr);
      diffuseNr++;

      // Set weights (simple example: equal weight)
      diffuseWeights[diffuseNr - 2] =
          1.0f; // diffuseNr started at 1, array at 0
    }

    // Set the sampler uniform (texture_diffuse1, texture_specular1, etc.)
    shader.setInt((name + number).c_str(), i);
    textures[i].texture.bind(GL_TEXTURE0 + i);
  }

  // Set diffuse count and weights uniforms
  shader.setInt("diffuseCount", diffuseNr - 1);
  shader.setFloatArray("diffuseWeights", diffuseWeights, diffuseNr - 1);

  // Draw call
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // A great thing about structs is that their memory layout is sequential for
  // all its items. The effect is that we can simply pass a pointer to the
  // struct and it translates perfectly to a glm::vec3/2 array which again
  // translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));
  // vertex bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));
  // ids
  glEnableVertexAttribArray(5);
  glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
                         (void *)offsetof(Vertex, m_BoneIDs));

  // weights
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, m_Weights));
  glBindVertexArray(0);
}
