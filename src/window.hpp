#pragma once
#include "GL/glew.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <cstdint>
#include <string>

class Window {
public:
  Window(const std::string &title, int width, int height);
  ~Window();

  void setPixel(int x, int y, uint32_t color);

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  void clear(uint32_t color);

  void present();
  void erase();

  int height;
  int width;

  bool running = true;

  SDL_Window *m_window = nullptr;

  SDL_GLContext glContext;
};