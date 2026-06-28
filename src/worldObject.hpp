#pragma once
#include "float3.hpp"
#include "quat.hpp"
#include <unordered_map>
#include <vector>

class Component;

using TypeId = uintptr_t;

template <typename T> inline TypeId getComponentTypeId() {
  static const char dummy = 0;
  return reinterpret_cast<TypeId>(&dummy);
}

class Entity {
public:
  Entity();
  Entity(float3 pos);
  ~Entity() = default;

  float3 pos{};
  float3 scale = float3(1, 1, 1, 0);
  Quat rotation{};

  int id;

  std::unordered_map<TypeId, int> componentMap;

  int lastUpdate;

  void updateModelMatrix();
  void translate(float3 delta);
  void rotate(float angle, float3 axis);
  const float *getModelMatrix();
  void update();
};

class Component {
public:
  int id;
  TypeId typeId;

  int ownerId;

  int lastUpdate;

  Component() = default;
  ~Component() = default;
};