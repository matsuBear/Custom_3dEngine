#pragma once
#include "context.hpp"
#include "float3.hpp"
#include "inputInterface.hpp"
#include "worldObject.hpp"

class Light;

class Light : public Component {
public:
  float3 color;
  float strength = 1;

  Light() = default;
  Light(float3 color, float strength);
};
