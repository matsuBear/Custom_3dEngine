#pragma once
#include "float3.hpp"
#include "scene.hpp"
#include <vector>

class RenderCache;

class RenderCache {
public:
  RenderCache() = default;
  std::vector<float3> lightPosCache;
  std::vector<float3> lightColorCache;
  int lightSize = 0;

  Scene *scenePtr = nullptr;

  void buildLightCaches();
};