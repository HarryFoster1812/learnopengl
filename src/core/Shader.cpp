#include <core/Shader.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::ifstream vFile(vertexPath), fFile(fragmentPath);
  std::stringstream vStream, fStream;

  if (!vFile.is_open() || !fFile.is_open()) {
    std::cerr << "ERROR: Could not open shader files: " << vertexPath << " or "
              << fragmentPath << std::endl;
    exit(EXIT_FAILURE);
  }

  vStream << vFile.rdbuf();
  fStream << fFile.rdbuf();

  GLuint vShader = compileShader(GL_VERTEX_SHADER, vStream.str(), vertexPath);
  GLuint fShader =
      compileShader(GL_FRAGMENT_SHADER, fStream.str(), fragmentPath);

  ID = glCreateProgram();
  glAttachShader(ID, vShader);
  glAttachShader(ID, fShader);
  glLinkProgram(ID);

  int success;
  char infoLog[512];
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, nullptr, infoLog);
    std::cerr << "ERROR: Shader program linking failed\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

Shader::~Shader() { glDeleteProgram(ID); }

GLuint Shader::compileShader(GLenum type, const std::string &source,
                             const std::string &filename) {
  GLuint shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "ERROR: Compilation failed for " << filename << "\n"
              << infoLog << std::endl;
  }

  return shader;
}

void Shader::use() const { glUseProgram(ID); }

GLint Shader::getUniformLocation(const std::string &name) {
  if (uniformLocationCache.find(name) != uniformLocationCache.end())
    return uniformLocationCache[name];

  GLint location = glGetUniformLocation(ID, name.c_str());
  if (location == -1)
    std::cerr << "Warning: Uniform '" << name << "' doesn't exist in shader.\n";
  uniformLocationCache[name] = location;
  return location;
}

void Shader::setBool(const std::string &name, bool value) {
  glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(const std::string &name, int value) {
  glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, float value) {
  glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string &name, float x, float y) {
  glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) {
  glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string &name, float x, float y, float z,
                     float w) {
  glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) {
  glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

// GLM overloads

void Shader::setVec2(const std::string &name, const glm::vec2 &vec) {
  setVec2(name, vec.x, vec.y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) {
  setVec3(name, vec.x, vec.y, vec.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &vec) {
  setVec4(name, vec.x, vec.y, vec.z, vec.w);
}
