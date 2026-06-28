#pragma once
#include <SDL2/SDL.h>
#include <cstdint>
#include <stdexcept>
#include <string>

class Window {
public:
  Window(const std::string &title, int width, int height);
  ~Window();

  // No copying — SDL resources are not copyable
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  // --- Pixel API ---
  // Color a single pixel. Origin (0,0) is top-left.
  void setPixel(int x, int y, uint32_t color);

  // Fill the entire buffer with one color (default: black)
  void clear(uint32_t color);

  // Push the CPU buffer to the screen. Call once per frame.
  void present();

  // --- Event loop ---
  // Returns false when the user closes the window.
  bool pollEvents();

  // --- Keyboard state ---
  // Check if a key is currently held down.
  // Use SDL_SCANCODE_* constants, e.g. SDL_SCANCODE_W
  bool isKeyDown(SDL_Scancode key) const;

  // --- Accessors ---
  int width() const { return m_width; }
  int height() const { return m_height; }

  // Raw pixel buffer — uint32_t per pixel, format 0x00RRGGBB
  // Useful if you want to write rows directly (e.g. memcpy a scanline)
  uint32_t *pixels() { return m_pixels; }

  int m_width;
  int m_height;
  bool running = true;
  uint32_t *m_pixels = nullptr;
  double *m_zBuffer = nullptr;

  double mouseSense = 0.003;

  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  SDL_Texture *m_texture = nullptr;
};
