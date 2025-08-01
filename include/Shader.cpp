#include "Shader.hpp"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

Shader::Shader(const char *vShaderPath, const char *fShaderPath) {
  std::ifstream vShaderFile(vShaderPath);
  std::ifstream fShaderFile(fShaderPath);
  std::string vSource, fSource;

  if (!vShaderFile.is_open()) {
    std::cout << "Failed to open vertex shader file\n";
  }
  if (!fShaderFile.is_open()) {
    std::cout << "Failed to open fragment shader file\n";
  }

  std::stringstream vShaderStream, fShaderStream;
  vShaderStream << vShaderFile.rdbuf();
  fShaderStream << fShaderFile.rdbuf();

  vSource = vShaderStream.str();
  fSource = fShaderStream.str();

  ID = compileShader(vSource.c_str(), fSource.c_str());
}

int Shader::compileShader(const char *vSourceCode, const char *fSourceCode) {
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vSourceCode, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fSourceCode, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

void Shader::use() { glUseProgram(ID); }
void Shader::setInt(const std::string &name, int value) const {

  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setV2(const std::string &name, float value1, float value2) const {

  glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
}
