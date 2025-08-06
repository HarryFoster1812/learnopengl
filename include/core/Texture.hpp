#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>

class Texture {
public:
  Texture(const std::string &path, GLenum textureType = GL_TEXTURE_2D,
          GLenum textureUnit = GL_TEXTURE0, bool flip = true);
  ~Texture();

  void bind() const;
  void unbind() const;

  void setWrap(GLenum s, GLenum t);
  void setFilter(GLenum minFilter, GLenum magFilter);

  GLuint getID() const { return ID; }
  GLenum getUnit() const { return textureUnit; }

private:
  GLuint ID;
  GLenum textureUnit;
  GLenum type;
  int width, height, nrChannels;
};

#endif
