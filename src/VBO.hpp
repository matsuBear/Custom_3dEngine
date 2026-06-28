#pragma once
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glext.h>

class VBO {
public:
  GLuint ID;

  VBO() = default;
  VBO(GLfloat *vertices, GLsizeiptr size);

  void set(GLfloat *vertices, GLsizeiptr size);
  void bind();
  void unbind();
  void exclude();
};