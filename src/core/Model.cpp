#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <core/Model.hpp>
#include <iostream>

Model::Model(const std::string &path) { loadModel(path); }

void Model::Draw(Shader &shader) {
  for (auto &mesh : meshes)
    mesh.Draw(shader);
}

void Model::loadModel(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<MaterialTexture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
                                mesh->mVertices[i].z);

    vertex.Normal = mesh->HasNormals()
                        ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
                                    mesh->mNormals[i].z)
                        : glm::vec3(0.0f);

    if (mesh->mTextureCoords[0]) {
      vertex.TexCoords =
          glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

      vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y,
                                 mesh->mTangents[i].z);

      vertex.Bitangent =
          glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
                    mesh->mBitangents[i].z);
    } else {
      vertex.TexCoords = glm::vec2(0.0f);
    }

    vertices.push_back(vertex);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<MaterialTexture> diffuseMaps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  std::vector<MaterialTexture> specularMaps = loadMaterialTextures(
      material, aiTextureType_SPECULAR, "texture_specular");
  std::vector<MaterialTexture> normalMaps =
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  std::vector<MaterialTexture> heightMaps =
      loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");

  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<MaterialTexture>
Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                            const std::string &typeName) {
  std::vector<MaterialTexture> textures;

  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string texturePath = str.C_Str();

    bool skip = false;
    for (const auto &loaded : textures_loaded) {
      if (loaded.path == texturePath) {
        textures.push_back(loaded);
        skip = true;
        break;
      }
    }

    if (!skip) {
      Texture tex(directory + "/" + texturePath);
      MaterialTexture matTex;
      matTex.texture = tex;
      matTex.type = typeName;
      matTex.path = texturePath;

      textures.push_back(matTex);
      textures_loaded.push_back(matTex);
    }
  }

  return textures;
}
