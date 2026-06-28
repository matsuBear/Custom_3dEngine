#include "scene.hpp"
#include "camera.hpp"
#include "eventHandler.hpp"
#include "float3.hpp"
#include "model.hpp"
#include "movement.hpp"
#include "worldObject.hpp"
#include <SDL2/SDL_timer.h>
#include <utility>

Scene::Scene(int height, int width) {
  this->height = height;
  this->width = width;
  aspectRatio = (float)width / height;
  projection.d[0] = 1 / (std::tan(FOV) * aspectRatio);
  projection.d[1] = 0;
  projection.d[2] = 0;
  projection.d[3] = 0;
  projection.d[4] = 0;
  projection.d[5] = 1 / std::tan(FOV);
  projection.d[6] = 0;
  projection.d[7] = 0;
  projection.d[8] = 0;
  projection.d[9] = 0;
  projection.d[10] = (fFar + fNear) / (fNear - fFar);
  projection.d[11] = 1;
  projection.d[12] = 0;
  projection.d[13] = 0;
  projection.d[14] = -2 * fNear * fFar / (fNear - fFar);
  projection.d[15] = 0;

  allPools.push_back(&entityList);
  allPools.push_back(&modelList);
  allPools.push_back(&lightList);
  allPools.push_back(&meshList);
  allPools.push_back(&orbitalList);
  allPools.push_back(&digiList);
}

Scene::~Scene(void) {}

void Scene::updateTime() {
  currentTime = SDL_GetTicks() / 1000.0f;
  deltaTime = (double)(currentTime - timeElapsed);
  timeElapsed = currentTime;
  fCounter++;
}

int Scene::returnFreeIdEntity() {
  if (!entityFreeIds.empty()) {
    int id = entityFreeIds.top();
    entityFreeIds.pop();
    return id;
  } else {
    entityLookUpActive++;
    return entityLookUpActive - 1;
  }
}

int Scene::returnFreeIdComponent() {
  if (!componentFreeIds.empty()) {
    int id = componentFreeIds.top();
    componentFreeIds.pop();
    return id;
  } else {
    componentLookUpActive++;
    return componentLookUpActive - 1;
  }
}

void Scene::spawnEntity(float3 pos) {
  entityList.create(Entity(pos));
  int eIndex = entityList.active - 1;
  Entity &e = entityList.data[eIndex];
  e.id = entityFreeIds.top();
  entityLookUp[e.id] = eIndex;
}

void Scene::deleteEntity(int eId) {

  Entity *e = &entityList.data[entityLookUp[eId]];

  // A REFAIRE C EST MOCHE C EST TRES MOCHE

  static const TypeId meshType = getComponentTypeId<Mesh>();
  static const TypeId lightType = getComponentTypeId<Light>();
  static const TypeId modelType = getComponentTypeId<Model>();
  static const TypeId camType = getComponentTypeId<Camera>();
  static const TypeId orbitalType = getComponentTypeId<OrbitalMovement>();

  for (std::pair<TypeId, int> pair : e->componentMap) {
    TypeId cType = pair.first;
    int cId = pair.second;

    int idToDeleteInPool = componentLookUp[cId];

    if (cType == meshType) {
      auto &pool = getPool(static_cast<Mesh *>(nullptr));
      int lastItemIndex = pool.active - 1;
      if (idToDeleteInPool != lastItemIndex) {
        componentLookUp[pool.data[lastItemIndex].id] = idToDeleteInPool;
      }
      pool.popAndSwitch(idToDeleteInPool);
    } else if (cType == lightType) {
      auto &pool = getPool(static_cast<Light *>(nullptr));
      int lastItemIndex = pool.active - 1;
      if (idToDeleteInPool != lastItemIndex) {
        componentLookUp[pool.data[lastItemIndex].id] = idToDeleteInPool;
      }
      pool.popAndSwitch(idToDeleteInPool);
    } else if (cType == modelType) {
      auto &pool = getPool(static_cast<Model *>(nullptr));
      int lastItemIndex = pool.active - 1;
      if (idToDeleteInPool != lastItemIndex) {
        componentLookUp[pool.data[lastItemIndex].id] = idToDeleteInPool;
      }
      pool.popAndSwitch(idToDeleteInPool);
    }
  }

  int idToDeleteInPool = entityLookUp[eId];

  int lastItemIndex = entityList.active - 1;

  if (idToDeleteInPool != lastItemIndex) {
    int toSwitchLook = entityList.data[lastItemIndex].id;
    componentLookUp[toSwitchLook] = idToDeleteInPool;
  }

  entityList.popAndSwitch(idToDeleteInPool);

  entityFreeIds.push(eId);
}

InputBus Scene::createBus(Entity *slcEntPtr, Entity *camEntPtr) {
  InputBus newBus{};
  for (std::pair compDict : slcEntPtr->componentMap) {
  }

  return newBus;
}

void Scene::updateAll() { updateTime(); }
