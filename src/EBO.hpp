#pragma once
#include <GL/glew.h>

class EBO {
public:
  GLuint ID;

  EBO() = default;
  EBO(GLint *indices, GLsizeiptr size, int meshType);

  void set(GLint *indices, GLsizeiptr size, int meshType);
  void bind();
  void unbind();
  void exclude();
};