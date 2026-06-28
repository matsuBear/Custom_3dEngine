#include "VAO.hpp"
#include "float3.hpp"
#include "mesh.hpp"

VAO::VAO() { glGenVertexArrays(1, &id); }

void VAO::linkVBO(VBO vbo, GLuint layout) {
  vbo.bind();
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex),
                        (void *)sizeof(float3));
  glEnableVertexAttribArray(1);
  vbo.unbind();
}

void VAO::bind() { glBindVertexArray(id); }

void VAO::unbind() { glBindVertexArray(0); }

void VAO::exclude() { glDeleteVertexArrays(1, &id); }