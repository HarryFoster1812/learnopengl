#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
  Shader(const char *vertexPath, const char *fragmentPath);
  ~Shader();

  void use() const;

  // Uniform utility
  void setBool(const std::string &name, bool value);
  void setInt(const std::string &name, int value);
  void setFloat(const std::string &name, float value);
  void setVec2(const std::string &name, float x, float y);
  void setVec3(const std::string &name, float x, float y, float z);
  void setVec4(const std::string &name, float x, float y, float z, float w);
  void setMat4(const std::string &name, const glm::mat4 &mat);

  GLuint getID() const { return ID; }

  // Disable copying
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

private:
  GLuint ID;
  std::unordered_map<std::string, GLint> uniformLocationCache;

  GLuint compileShader(GLenum type, const std::string &source,
                       const std::string &filename);
  GLint getUniformLocation(const std::string &name);
};
