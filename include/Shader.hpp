#pragma once
#include "../glad/glad.h" // include glad to get the required OpenGL headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
private:
  int compileShader(const char *vSourceCode, const char *fSourceCode);

public:
  unsigned int ID;
  Shader(const char *vShaderPath, const char *fShaderPath);
  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
};
