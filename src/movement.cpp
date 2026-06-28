#include "movement.hpp"

#include "config.hpp"
#include "context.hpp"
#include "memPool.hpp"
void DigiMovement::integrate() {}

OrbitalMovement::OrbitalMovement(int centerId) { this->centerId = centerId; }

void OrbitalMovement::downAction() { elevation -= 1; }

void OrbitalMovement::upAction() { elevation += 1; }

void OrbitalMovement::rightAction() { azimuth += 1; }

void OrbitalMovement::leftAction() { azimuth -= 1; }

void OrbitalMovement::forwardAction() { radius -= 5; }

void OrbitalMovement::backwardAction() { radius += 5; }

void OrbitalMovement::integrate() {}

namespace PhysicsSystem {
template <typename MoveComp>
void updateMovement(memPool<Entity, MAX_MEMPOOL_SIZE> &ePool, int *entityLookUp,
                    memPool<MoveComp, MAX_MEMPOOL_SIZE> &movePool,
                    const Context &contx) {
  for (int i = 0; i < movePool.active; ++i) {
    MoveComp &move = movePool.data[i];
    if (move.velocity.x != 0.0f || move.velocity.y != 0.0f ||
        move.velocity.z != 0.0f) {
      Entity &e = ePool.data[entityLookUp[move.ownerId]];
      e.pos += move.velocity;
      e.lastUpdate = contx.currentFrame;
    }
  }
}
template <>
void updateMovement<OrbitalMovement>(
    memPool<Entity, MAX_MEMPOOL_SIZE> &ePool, int *entityLookUp,
    memPool<OrbitalMovement, MAX_MEMPOOL_SIZE> &movePool,
    const Context &contx) {
  for (int i = 0; i < movePool.active; ++i) {
    OrbitalMovement &move = movePool.data[i];

    move.totalAzimuth += move.azimuth * contx.deltaTime;
    move.totalElevation += move.elevation * contx.deltaTime;
    move.totalElevation =
        std::max(-M_PIf / 2.01f, std::min(M_PIf / 2.01f, move.totalElevation));

    float azimuth = move.totalAzimuth;
    float radius = move.radius;
    float elevation = move.totalElevation;

    if (move.lastFrameAzimuth != azimuth ||
        move.lastFrameEleveation != elevation ||
        move.lastFrameRadius != radius) {

      move.lastFrameAzimuth = azimuth;
      move.lastFrameEleveation = elevation;
      move.lastFrameRadius = radius;

      Entity &e = ePool.data[entityLookUp[move.ownerId]];
      float3 center = ePool.data[entityLookUp[move.centerId]].pos;

      float3 &pos = e.pos;

      pos.x = center.x + radius * std::cos(elevation) * std::sin(azimuth);
      pos.y = center.y + radius * std::sin(elevation);
      pos.z = -(center.z + radius * std::cos(elevation) * std::cos(azimuth));

      e.lastUpdate = contx.currentFrame;

      move.azimuth = 0;
      move.elevation = 0;
    }
  }
}
} // namespace PhysicsSystem
