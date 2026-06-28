#include "light.hpp"
#include "float3.hpp"

Light::Light(float3 color, float strength) {
  this->color = color;
  this->strength = strength;
}