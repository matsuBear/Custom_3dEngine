#include "eventHandler.hpp"
#include "camera.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>

void InputHandler::poll(InputBus &bus) {
  SDL_Event e;
  mouseInput(bus, e);
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      bus.currentWindow->running = false;
    } else {
      instantInput(bus, e);
    }
  }
  continousInput(bus);
}

void InputHandler::instantInput(InputBus &bus, SDL_Event &event) {
  switch (event.key.keysym.scancode) {
  case quitButton:
    bus.currentWindow->running = false;
    break;
  default:
    break;
  }
}

void InputHandler::continousInput(InputBus &bus) {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  if (keys[forward])
    bus.selectedCam->forwardAction();
  if (keys[backward])
    bus.selectedCam->backwardAction();
  if (keys[rightward])
    bus.selectedCam->rightAction();
  if (keys[leftward])
    bus.selectedCam->leftAction();
  if (keys[downward])
    bus.selectedCam->downAction();
  if (keys[upward])
    bus.selectedCam->upAction();

  if (bus.selectedMovement) {
    if (keys[lightUp]) {
      if (keys[lShift]) {
        bus.selectedMovement->forwardAction();
      } else {
        bus.selectedMovement->upAction();
      }
    }
    if (keys[lightDown]) {
      if (keys[lShift]) {
        bus.selectedMovement->backwardAction();
      } else {
        bus.selectedMovement->downAction();
      }
    }
    if (keys[lightLeft]) {
      bus.selectedMovement->leftAction();
    }
    if (keys[lightRigth]) {
      bus.selectedMovement->rightAction();
    }
  }
}

void InputHandler::mouseInput(InputBus &bus, SDL_Event &event) {
  bus.selectedCam->mouseMoveAction();
}