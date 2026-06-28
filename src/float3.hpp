#pragma once
#include <GL/glew.h>
#include <SDL2/SDL_pixels.h>

class float3;
class Matrix4x4;

class float3 {
public:
  float3();
  float3(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

  GLfloat size();

  float3 &operator=(float3 u);
  float3 operator+(float3 u);
  float3 operator-(float3 u);
  float3 operator*(GLfloat q);
  float3 operator/(GLfloat q);
  void operator+=(float3 u);
  void operator-=(float3 u);
  void operator*=(GLfloat q);
  void operator/=(GLfloat q);
  float3 operator*(Matrix4x4 m);
  bool operator==(float3 u);

  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
};

void normalise(float3 &u);
GLfloat dot(float3 u, float3 v);
float3 cross(float3 u, float3 v);
float3 getColor(float r, float g, float b, float a);
float3 rotatePointAxisAngle(float3 point, float3 axis, float angle);

class Matrix4x4 {
public:
  Matrix4x4() = default;
  float d[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
};
