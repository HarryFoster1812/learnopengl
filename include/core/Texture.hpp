#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>

class Texture {
public:
  Texture(const std::string &path, GLenum textureType = GL_TEXTURE_2D,
          bool flip = true);
  ~Texture();

  void bind(GLenum textureUnit = GL_TEXTURE0) const;
  void unbind() const;

  void setWrap(GLenum s, GLenum t);
  void setFilter(GLenum minFilter, GLenum magFilter);

  GLuint getID() const { return ID; }

private:
  GLuint ID;
  GLenum type;
  int width, height, nrChannels;
};

#endif
