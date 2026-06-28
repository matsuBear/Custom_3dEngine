#include "VBO.hpp"
#include "mesh.hpp"
#include <GL/glext.h>

VBO::VBO(GLfloat *vertices, GLsizeiptr size) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ARRAY_BUFFER, ID);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(Mesh::Vertex), vertices,
               GL_STATIC_DRAW);
}

void VBO::set(GLfloat *vertices, GLsizeiptr size) {
  glGenBuffers(1, &ID);
  glBindBuffer(GL_ARRAY_BUFFER, ID);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(Mesh::Vertex), vertices,
               GL_STATIC_DRAW);
}

void VBO::bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }

void VBO::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VBO::exclude() { glDeleteBuffers(1, &ID); }
