#pragma once
#include "context.hpp"
#include "float3.hpp"
#include "inputInterface.hpp"
#include "worldObject.hpp"

class Camera : public Component {
public:
  float3 xAxis;
  float3 yAxis;
  float3 zAxis;

  float yaw = 0;
  float pitch = 0;
  float roll = 0;

  int dx = 0;
  int dy = 0;

  Matrix4x4 view;

  float3 velocity{};

  Camera();
  void mouseMoveAction();
  void rightAction();
  void leftAction();
  void upAction();
  void downAction();
  void forwardAction();
  void backwardAction();
};