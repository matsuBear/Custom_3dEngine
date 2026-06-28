#pragma once

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"
#include "movement.hpp"
#include "window.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>

class InputBus;

const SDL_Scancode forward = SDL_SCANCODE_W;
const SDL_Scancode backward = SDL_SCANCODE_S;
const SDL_Scancode rightward = SDL_SCANCODE_D;
const SDL_Scancode leftward = SDL_SCANCODE_A;
const SDL_Scancode upward = SDL_SCANCODE_SPACE;
const SDL_Scancode downward = SDL_SCANCODE_X;
const SDL_Scancode meshCycleUp = SDL_SCANCODE_2;
const SDL_Scancode meshCycleDown = SDL_SCANCODE_1;
const SDL_Scancode lightUp = SDL_SCANCODE_UP;
const SDL_Scancode lightDown = SDL_SCANCODE_DOWN;
const SDL_Scancode lightRigth = SDL_SCANCODE_RIGHT;
const SDL_Scancode lightLeft = SDL_SCANCODE_LEFT;
const SDL_Scancode quitButton = SDL_SCANCODE_ESCAPE;
const SDL_Scancode lShift = SDL_SCANCODE_LSHIFT;

const float PI = 3.14159265358979323846;

class InputHandler {
public:
  InputHandler() = default;

  bool keyState[SDL_NUM_SCANCODES] = {};
  bool quit = false;
  float moveSpeed = 2;
  float mouseSense = 30;

  void poll(InputBus &bus);

  void instantInput(InputBus &bus, SDL_Event &event);

  void continousInput(InputBus &bus);

  void mouseInput(InputBus &bus, SDL_Event &event);
};

class InputBus {
public:
  Camera *selectedCam{};
  Mesh *selectedMesh{};
  Light *selectedLight{};
  Movement *selectedMovement{};
  Window *currentWindow{};
};