#pragma once
#include "float3.hpp"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using std::string;

string shaderLoader(const string &path);

class Shader {
public:
  Shader(const string &vertexShader, const string &fragmentShader);
  void activate();
  void exclude();
  float buildProjection(float FOV, float aspectRatio, int height, int width);

  GLuint id;
};