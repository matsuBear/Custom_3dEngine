#include "worldObject.hpp"
#include "float3.hpp"
#include <utility>

Entity::Entity() { this->pos = float3(); }

Entity::Entity(float3 pos) { this->pos = pos; }

void Entity::translate(float3 delta) {
  pos.x += delta.x;
  pos.y += delta.y;
  pos.z += delta.z;
}

void Entity::rotate(float angle, float3 axis) {
  Quat delta = Quat::angleAxisQuat(angle, axis);
  this->rotation = delta * this->rotation;
}