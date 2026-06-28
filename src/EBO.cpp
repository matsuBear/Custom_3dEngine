#include "EBO.hpp"
#include "float3.hpp"
#include "mesh.hpp"
#include <GL/gl.h>

EBO::EBO(GLint *indices, GLsizeiptr size, int meshType) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), indices,
               meshType);
}
void EBO::set(GLint *indices, GLsizeiptr size, int meshType) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), indices,
               meshType);
}

void EBO::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }

void EBO::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

void EBO::exclude() { glDeleteBuffers(1, &ID); }