#include "model.hpp"
#include "memPool.hpp"
#include "scene.hpp"

namespace ModelSystem {
void updateModelMatrix(memPool<Model, MAX_MEMPOOL_SIZE> modelPool,
                       int *componentLookUp,
                       memPool<Entity, MAX_MEMPOOL_SIZE> ePool,
                       int *entityLookUp, const Context &contx) {
  for (int i = 0; i < modelPool.active; i++) {
    Model &m = modelPool.data[i];
    Entity &e = ePool.data[entityLookUp[m.ownerId]];
    if (e.lastUpdate == contx.currentFrame) {

      float3 scale = e.scale;
      float3 pos = e.pos;
      Quat rotation = e.rotation;

      float r = rotation.r;
      float x = rotation.x;
      float y = rotation.y;
      float z = rotation.z;

      float xx = x * x;
      float yy = y * y;
      float zz = z * z;
      float xy = x * y;
      float xz = x * z;
      float yz = y * z;
      float rx = r * x;
      float ry = r * y;
      float rz = r * z;

      m.d[0] = (1.0f - 2.0f * (yy + zz)) * scale.x;
      m.d[1] = (2.0f * (xy + rz)) * scale.x;
      m.d[2] = (2.0f * (xz - ry)) * scale.x;
      m.d[3] = 0.0f;
      m.d[4] = (2.0f * (xy - rz)) * scale.y;
      m.d[5] = (1.0f - 2.0f * (xx + zz)) * scale.y;
      m.d[6] = (2.0f * (yz + rx)) * scale.y;
      m.d[7] = 0.0f;
      m.d[8] = (2.0f * (xz + ry)) * scale.z;
      m.d[9] = (2.0f * (yz - rx)) * scale.z;
      m.d[10] = (1.0f - 2.0f * (xx + yy)) * scale.z;
      m.d[11] = 0.0f;
      m.d[12] = pos.x;
      m.d[13] = pos.y;
      m.d[14] = pos.z;
      m.d[15] = 1.0f;
    }
  }
}
} // namespace ModelSystem