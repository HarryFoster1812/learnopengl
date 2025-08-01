#pragma once
#include "../glad/glad.h" // include glad to get the required OpenGL headers
#include <string>

class Shader {

public:
  unsigned int ID;
  Shader(const char *vShaderPath, const char *fShaderPath);
  int compileShader(const char *vSourceCode, const char *fSourceCode);
  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setV2(const std::string &name, float value1, float value2) const;
};
