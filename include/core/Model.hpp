#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stb_image.h>

#include <core/Mesh.hpp>
#include <core/Shader.hpp>
#include <core/Texture.hpp>

#include <map>
#include <string>
#include <vector>

class Model {
public:
  Model(const std::string &path);

  void Draw(Shader &shader);

private:
  std::vector<Mesh> meshes;
  std::vector<MaterialTexture> textures_loaded;
  std::string directory;

  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<MaterialTexture>
  loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                       const std::string &typeName);
};
