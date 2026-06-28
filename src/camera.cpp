#include "camera.hpp"
#include "context.hpp"
#include "float3.hpp"
#include "memPool.hpp"
#include "scene.hpp"
#include "worldObject.hpp"
#include <SDL2/SDL_mouse.h>
#include <cmath>
#include <utility>

Camera::Camera() {
  SDL_SetRelativeMouseMode(SDL_TRUE);
  this->xAxis = float3(1, 0, 0, 0);
  this->yAxis = float3(0, 1, 0, 0);
  this->zAxis = float3(0, 0, 1, 0);
}

void Camera::mouseMoveAction() { SDL_GetRelativeMouseState(&dx, &dy); }

void Camera::rightAction() { velocity += xAxis; }

void Camera::leftAction() { velocity -= xAxis; }

void Camera::forwardAction() { velocity += zAxis; }

void Camera::backwardAction() { velocity -= zAxis; }

void Camera::downAction() { velocity -= float3{0, 1, 0, 0}; }

void Camera::upAction() { velocity += float3{0, 1, 0, 0}; }

namespace CameraSystem {
void generateZ(memPool<Camera, MAX_MEMPOOL_SIZE> camPool,
               memPool<Entity, MAX_MEMPOOL_SIZE> ePool, int *componentLookUp,
               int *entityLookUp, int selecCamId, const Context &contx) {

  Camera &cam = camPool.data[componentLookUp[selecCamId]];
  Entity &e = ePool.data[entityLookUp[cam.ownerId]];

  if (cam.dx != 0 || cam.dy != 0) {
    int dx = contx.mouseMove.first;
    int dy = contx.mouseMove.second;
    cam.yaw += dx * contx.mouseSense * contx.deltaTime;
    cam.pitch -= dy * contx.mouseSense * contx.deltaTime;

    cam.pitch = std::max(-M_PIf / 1.99f, std::min(M_PIf / 2.01f, cam.pitch));

    cam.zAxis.x = cos(cam.pitch) * sin(cam.yaw);
    cam.zAxis.y = sin(cam.pitch);
    cam.zAxis.z = cos(cam.pitch) * cos(cam.yaw);
    normalise(cam.zAxis);

    cam.dx = 0;
    cam.dy = 0;
  }
}

void generateView(memPool<Camera, MAX_MEMPOOL_SIZE> camPool,
                  memPool<Entity, MAX_MEMPOOL_SIZE> ePool, int *componentLookUp,
                  int *entityLookUp, int selecCamId, const Context &contx) {

  Camera &cam = camPool.data[componentLookUp[selecCamId]];
  Entity &e = ePool.data[entityLookUp[cam.ownerId]];

  float3 worldUp(0, 1, 0, 0);
  float3 pos = e.pos;

  cam.xAxis = cross(worldUp, cam.zAxis);
  normalise(cam.xAxis);

  cam.yAxis = cross(cam.zAxis, cam.xAxis);
  normalise(cam.yAxis);

  cam.view.d[0] = cam.xAxis.x;
  cam.view.d[1] = cam.yAxis.x;
  cam.view.d[2] = cam.zAxis.x;
  cam.view.d[3] = 0;
  cam.view.d[4] = cam.xAxis.y;
  cam.view.d[5] = cam.yAxis.y;
  cam.view.d[6] = cam.zAxis.y;
  cam.view.d[7] = 0;
  cam.view.d[8] = cam.xAxis.z;
  cam.view.d[9] = cam.yAxis.z;
  cam.view.d[10] = cam.zAxis.z;
  cam.view.d[11] = 0;
  cam.view.d[12] = -dot(pos, cam.xAxis);
  cam.view.d[13] = -dot(pos, cam.yAxis);
  cam.view.d[14] = -dot(pos, cam.zAxis);
  cam.view.d[15] = 1;
}

void integrate(memPool<Camera, MAX_MEMPOOL_SIZE> camPool,
               memPool<Entity, MAX_MEMPOOL_SIZE> ePool, int *componentLookUp,
               int *entityLookUp, int selecCamId, const Context &contx) {

  Camera &cam = camPool.data[componentLookUp[selecCamId]];
  Entity &e = ePool.data[entityLookUp[cam.ownerId]];

  if (cam.velocity.size() > 0) {
    e.pos += cam.velocity * contx.deltaTime;
    cam.velocity.x = 0;
    cam.velocity.y = 0;
    cam.velocity.z = 0;
    e.lastUpdate = contx.currentFrame;
  }
}
} // namespace CameraSystem