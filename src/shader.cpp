#include "shader.hpp"
#include "float3.hpp"
#include <cstddef>
#include <iostream>

using std::string;

std::string shaderLoader(const std::string &path) {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Shader::Shader(const string &vertexFile, const string &fragmentFile) {
  string vertexString = shaderLoader(vertexFile);
  string fragmentString = shaderLoader(fragmentFile);

  const char *vertexSource = vertexString.c_str();
  const char *fragmentSource = fragmentString.c_str();

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  GLint success;
  char log[512];

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, log);
    std::cerr << log << std::endl;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, log);
    std::cerr << log << std::endl;
  }

  id = glCreateProgram();

  glAttachShader(id, vertexShader);
  glAttachShader(id, fragmentShader);

  glLinkProgram(id);

  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, NULL, log);
    std::cerr << log << std::endl;
  }

  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
}

void Shader::activate() { glUseProgram(id); }

void Shader::exclude() { glDeleteProgram(id); }