#include <core/Texture.hpp>
#include <iostream>
#include <stb/stb_image.h>

Texture::Texture()
    : ID(0), type(GL_TEXTURE_2D), width(0), height(0), nrChannels(0) {}

Texture::Texture(const std::string &path, GLenum textureType, bool flip)
    : type(textureType), width(0), height(0), nrChannels(0) {

  std::cout << "Loading Texture from path: " << path << std::endl;

  glGenTextures(1, &ID);
  glBindTexture(type, ID);

  // Default texture settings
  setWrap(GL_REPEAT, GL_REPEAT);
  setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  if (flip) {
    stbi_set_flip_vertically_on_load(true);
  }

  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (data) {
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;
    else
      format = GL_RGB;

    glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(type);
  } else {
    std::cerr << "Failed to load texture at path: " << path << std::endl;
  }

  stbi_image_free(data);
  glBindTexture(type, 0);
}

void Texture::bind(GLenum textureUnit) const {
  glActiveTexture(textureUnit);
  glBindTexture(type, ID);
}

void Texture::unbind() const { glBindTexture(type, 0); }

void Texture::setWrap(GLenum s, GLenum t) {
  glTexParameteri(type, GL_TEXTURE_WRAP_S, s);
  glTexParameteri(type, GL_TEXTURE_WRAP_T, t);
}

void Texture::setFilter(GLenum minFilter, GLenum magFilter) {
  glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
}
