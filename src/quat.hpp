#pragma once
#include "float3.hpp"
class Quat {
public:
  Quat();
  Quat(float r, float x, float y, float z);
  Quat operator*(Quat q);
  Quat operator+(Quat q);
  float3 operator*(float3);

  float r;
  float x;
  float y;
  float z;

  static Quat angleAxisQuat(float angle, float3 axis);
  static Quat fromEuler(float pitchDegrees, float yawDegrees,
                        float rollDegrees);
};