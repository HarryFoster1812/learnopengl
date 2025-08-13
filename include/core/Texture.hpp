#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
  Texture();
  Texture(const std::string &path, GLenum textureType = GL_TEXTURE_2D,
          bool flip = true);

  void bind(GLenum textureUnit) const;
  void unbind() const;

  void setWrap(GLenum s, GLenum t);
  void setFilter(GLenum minFilter, GLenum magFilter);

  GLuint getID() const { return ID; }

private:
  GLuint ID = 0;
  GLenum type;
  int width, height, nrChannels;
};
