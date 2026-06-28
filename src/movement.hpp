#pragma once
#include "float3.hpp"
#include "worldObject.hpp"

class Movement {
public:
  virtual ~Movement() = default;

  virtual void leftAction() = 0;
  virtual void rightAction() = 0;
  virtual void upAction() = 0;
  virtual void downAction() = 0;
  virtual void forwardAction() = 0;
  virtual void backwardAction() = 0;
};

class DigiMovement : public Component, public Movement {
public:
  DigiMovement();
  ~DigiMovement() override = default;

  float3 velocity{};
  void integrate();

  void leftAction() override;
  void rightAction() override;
  void upAction() override;
  void downAction() override;
  void forwardAction() override;
  void backwardAction() override;
};

class OrbitalMovement : public Component, public Movement {
public:
  OrbitalMovement() = default;
  OrbitalMovement(int centerId);
  ~OrbitalMovement() override = default;

  float radius = 0;

  float totalAzimuth = 0;
  float totalElevation = 0;
  float azimuth = 0;
  float elevation = 0;

  float lastFrameAzimuth = 0;
  float lastFrameEleveation = 0;
  float lastFrameRadius = 0;

  float3 lastPos{};
  float3 newPos{};

  int centerId;

  void leftAction() override;
  void rightAction() override;
  void upAction() override;
  void downAction() override;
  void forwardAction() override;
  void backwardAction() override;

  void assignCenter(int centerId) { this->centerId = id; }

  void integrate();

  void updatePos();
};
