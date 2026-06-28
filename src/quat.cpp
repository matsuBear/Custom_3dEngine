#include "quat.hpp"
#include "float3.hpp"
#include <cmath>

Quat::Quat() {
  this->r = 1.0f;
  this->x = 0.0f;
  this->y = 0.0f;
  this->z = 0.0f;
}

Quat::Quat(float r, float x, float y, float z) {
  this->r = r;
  this->x = x;
  this->y = y;
  this->z = z;
}

Quat Quat::operator+(Quat q) {
  return Quat(this->r + q.r, this->x + q.x, this->y + q.y, this->z + q.z);
}

Quat Quat::operator*(Quat q) {
  return Quat(this->r * q.r - this->x * q.x - this->y * q.y - this->z * q.z,
              this->r * q.x + this->x * q.r + this->y * q.z - this->z * q.y,
              this->r * q.y - this->x * q.z + this->y * q.r + this->z * q.x,
              this->r * q.z + this->x * q.y - this->y * q.x + this->z * q.r);
}

float3 Quat::operator*(float3 v) {

  float3 u(this->x, this->y, this->z, 0);

  float3 uv = cross(u, v);
  float3 uuv = cross(u, uv);
  float3 result;

  result.x = v.x + 2.0f * (this->r * uv.x + uuv.x);
  result.y = v.y + 2.0f * (this->r * uv.y + uuv.y);
  result.z = v.z + 2.0f * (this->r * uv.z + uuv.z);
  result.w = 1;

  return result;
}

Quat Quat::angleAxisQuat(float angle, float3 axis) {

  float halfAngle = angle * 0.5f;
  float sinHalf = std::sin(halfAngle);
  float cosHalf = std::cos(halfAngle);

  return Quat(cosHalf, axis.x * sinHalf, axis.y * sinHalf, axis.z * sinHalf);
}

Quat Quat::fromEuler(float pitchDegrees, float yawDegrees, float rollDegrees) {
  float pitchRad = pitchDegrees * 0.01745329251f;
  float yawRad = yawDegrees * 0.01745329251f;
  float rollRad = rollDegrees * 0.01745329251f;

  float cp = std::cos(pitchRad * 0.5f);
  float sp = std::sin(pitchRad * 0.5f);
  float cy = std::cos(yawRad * 0.5f);
  float sy = std::sin(yawRad * 0.5f);
  float cr = std::cos(rollRad * 0.5f);
  float sr = std::sin(rollRad * 0.5f);

  Quat q;
  q.r = cy * cp * cr + sy * sp * sr;
  q.x = cy * sp * cr + sy * cp * sr;
  q.y = sy * cp * cr - cy * sp * sr;
  q.z = cy * cp * sr - sy * sp * cr;

  return q;
}