#pragma once
#include "camera.hpp"
#include "config.hpp"
#include "eventHandler.hpp"
#include "float3.hpp"
#include "light.hpp"
#include "memPool.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "movement.hpp"
#include "worldObject.hpp"
#include <GL/glew.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <array>
#include <stack>
#include <vector>

class Scene;

class Scene {
public:
  Scene(int height, int width);
  ~Scene(void);

  void updateTime();

  void spawnEntity(float3 pos);

  int returnFreeIdEntity();

  int returnFreeIdComponent();

  inline memPool<Mesh, MAX_MEMPOOL_SIZE> &getPool(Mesh *) { return meshList; }
  inline memPool<Light, MAX_MEMPOOL_SIZE> &getPool(Light *) {
    return lightList;
  }
  inline memPool<Model, MAX_MEMPOOL_SIZE> &getPool(Model *) {
    return modelList;
  }
  inline memPool<Camera, MAX_MEMPOOL_SIZE> &getPool(Camera *) {
    return cameraList;
  }

  template <typename Component>
  void spawnComponent(int ownerId, Component &temp) {
    auto &pool = getPool(static_cast<Component *>(nullptr));
    temp.id = returnFreeIdComponent();
    temp.ownerId = ownerId;
    entityList.data[entityLookUp[ownerId]].componentMap.insert(
        {getComponentTypeId<Component>(), temp.id});
    pool.create(temp);
  };

  template <typename Component> void deleteComponent(int eId) {

    auto &pool = getPool(static_cast<Component *>(nullptr));

    Entity *e = &entityList.data[entityLookUp[eId]];
    TypeId cType = getComponentTypeId<Component>();
    int idToDelete = e->componentMap.at(cType);
    int idToDeleteInPool = componentLookUp[idToDelete];

    e->componentMap.erase(cType);

    int lastItemIndex = pool.active - 1;

    if (idToDeleteInPool != lastItemIndex) {
      int toSwitchLook = pool.data[lastItemIndex].id;
      componentLookUp[toSwitchLook] = idToDeleteInPool;
    }

    pool.popAndSwitch(idToDeleteInPool);

    componentFreeIds.push(idToDelete);
  }

  void deleteEntity(int eId);

  InputBus createBus(Entity *slcEntPtr, Entity *camEntPtr);

  void updateAll();

  int height;
  int width;
  const float Pi = 2 * acos(0);
  float fNear = 1;
  float fFar = 1000;
  float FOV = Pi * 1 / 4;
  float aspectRatio;
  float deltaTime = 0;
  float currentTime = 0;
  float timeElapsed = 0;
  Uint32 fCounter = 0;

  float3 worldOrigin = float3(0, 0, 0, 1);
  float3 worldX = float3(1, 0, 0, 0);
  float3 worldY = float3(0, 1, 0, 0);
  float3 worldZ = float3(0, 0, 1, 0);

  float ambientLight = 0.2f;

  Matrix4x4 projection;

  std::array<int, MAX_MEMPOOL_SIZE> entityLookUp;
  int entityLookUpActive = 0;
  memPool<Entity, MAX_MEMPOOL_SIZE> entityList;
  std::stack<int> entityFreeIds;

  std::array<int, MAX_MEMPOOL_SIZE> componentLookUp;
  int componentLookUpActive = 0;
  std::stack<int> componentFreeIds;

  std::vector<BaseMemPool *> allPools;

  memPool<Mesh, MAX_MEMPOOL_SIZE> meshList;

  memPool<Light, MAX_MEMPOOL_SIZE> lightList;

  memPool<Model, MAX_MEMPOOL_SIZE> modelList;

  memPool<Camera, MAX_MEMPOOL_SIZE> cameraList;

  memPool<OrbitalMovement, MAX_MEMPOOL_SIZE> orbitalList;

  memPool<DigiMovement, MAX_MEMPOOL_SIZE> digiList;

  int selectedEntity;

  InputBus bus{};
};