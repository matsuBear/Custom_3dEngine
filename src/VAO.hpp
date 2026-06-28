#pragma once
#include "GL/glew.h"
#include "VBO.hpp"

class VAO {
public:
  GLuint id;
  VAO();

  void linkVBO(VBO vbo, GLuint layout);
  void bind();
  void unbind();
  void exclude();
};