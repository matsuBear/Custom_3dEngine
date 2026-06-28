#include "float3.hpp"
#include <GL/gl.h>
#include <algorithm>
#include <cmath>

float3::float3() {
  this->x = 0;
  this->y = 0;
  this->z = 0;
  this->w = 0;
}

float3::float3(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

GLfloat float3::size() { return std::sqrt(x * x + y * y + z * z); }

float3 &float3::operator=(float3 u) {
  this->x = u.x;
  this->y = u.y;
  this->z = u.z;
  this->w = u.w;
  return *this;
}

float3 float3::operator+(float3 u) {
  return float3(this->x + u.x, this->y + u.y, this->z + u.z,
                std::max(this->w, u.w));
}

float3 float3::operator-(float3 u) {
  return float3(this->x - u.x, this->y - u.y, this->z - u.z,
                std::max(this->w, u.w));
}

float3 float3::operator*(GLfloat q) {
  return float3(this->x * q, this->y * q, this->z * q, this->w);
}
float3 float3::operator/(GLfloat q) {
  return float3(this->x / q, this->y / q, this->z / q, this->w);
}
void float3::operator+=(float3 u) {
  this->x = this->x + u.x;
  this->y = this->y + u.y;
  this->z = this->z + u.z;
  this->w = std::max(this->w, u.w);
}
void float3::operator-=(float3 u) {
  this->x = this->x - u.x;
  this->y = this->y - u.y;
  this->z = this->z - u.z;
  this->w = std::max(this->w, u.w);
}
void float3::operator*=(GLfloat q) {
  this->x = this->x * q;
  this->y = this->y * q;
  this->z = this->z * q;
}
void float3::operator/=(GLfloat q) {
  this->x = this->x / q;
  this->y = this->y / q;
  this->z = this->z / q;
}

float3 float3::operator*(Matrix4x4 m) {
  float3 result;

  result.x = (m.d[0] * this->x) + (m.d[4] * this->y) + (m.d[8] * this->z) +
             (m.d[12] * this->w);
  result.y = (m.d[1] * this->x) + (m.d[5] * this->y) + (m.d[9] * this->z) +
             (m.d[13] * this->w);
  result.z = (m.d[2] * this->x) + (m.d[6] * this->y) + (m.d[10] * this->z) +
             (m.d[14] * this->w);
  result.w = (m.d[3] * this->x) + (m.d[7] * this->y) + (m.d[11] * this->z) +
             (m.d[15] * this->w);

  return result;
}

bool float3::operator==(float3 u) {
  return (this->x == u.x) && (this->y == u.y) && (this->z == u.z);
}

void normalise(float3 &u) { u /= u.size(); }

float3 cross(float3 u, float3 v) {
  return float3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x, 0);
}

GLfloat dot(float3 u, float3 v) { return u.x * v.x + u.y * v.y + u.z * v.z; }

float3 getColor(float r, float g, float b, float a) {
  return float3(r / 255, g / 255, b / 255, a / 255);
}

float3 rotatePointAxisAngle(float3 point, float3 axis, float angle) {
  float c = cosf(angle);
  float s = sinf(angle);

  float3 crossed = cross(axis, point);
  float dotted = dot(axis, point);
  float factor = dotted * (1.0f - c);

  float3 result;
  result.x = point.x * c + crossed.x * s + axis.x * factor;
  result.y = point.y * c + crossed.y * s + axis.y * factor;
  result.z = point.z * c + crossed.z * s + axis.z * factor;

  return result;
}
