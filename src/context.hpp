
#pragma once
#include "float3.hpp"
#include <utility>

class Context {
public:
  float mouseSense;
  std::pair<int, int> mouseMove;
  float3 camVelocity{};

  float deltaTime;
  int currentFrame;
};