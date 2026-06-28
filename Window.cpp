#include "Window.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>
#include <cstdint>

// ---- Constructor / Destructor ----------------------------------------

Window::Window(const std::string &title, int width, int height)
    : m_width(width), m_height(height) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

  m_window =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
  if (!m_window)
    throw std::runtime_error(std::string("SDL_CreateWindow failed: ") +
                             SDL_GetError());

  // SDL_RENDERER_PRESENTVSYNC caps at your monitor refresh rate (usually 60Hz).
  // Remove that flag if you want uncapped frames.
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (!m_renderer)
    throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") +
                             SDL_GetError());

  // ARGB8888: one uint32_t per pixel, layout 0xAARRGGBB.
  // We always write alpha=0xFF so the window is fully opaque.
  m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!m_texture)
    throw std::runtime_error(std::string("SDL_CreateTexture failed: ") +
                             SDL_GetError());

  m_pixels = new uint32_t[width * height];
  m_zBuffer = new double[width * height];
  std::fill(m_zBuffer, m_zBuffer + width * height,
            std::numeric_limits<double>::max());
  clear(0xFF000000);
}

Window::~Window() {
  delete[] m_pixels;
  delete[] m_zBuffer;
  SDL_DestroyTexture(m_texture);
  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

// ---- Pixel API -------------------------------------------------------

void Window::setPixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    return;
  // Pack into ARGB8888: alpha always 0xFF
  m_pixels[y * m_width + x] = color;
}

void Window::clear(uint32_t color) {
  uint32_t count = m_width * m_height;
  for (uint32_t i = 0; i < count; ++i) {
    m_pixels[i] = color;
    m_zBuffer[i] = std::numeric_limits<double>::max();
  }
}

void Window::present() {
  // Upload CPU buffer → GPU texture
  SDL_UpdateTexture(m_texture, nullptr, m_pixels, m_width * sizeof(uint32_t));
  // Blit the whole texture to the whole renderer
  SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
  // Swap buffers (vsync waits here if enabled)
  SDL_RenderPresent(m_renderer);
}

// ---- Events ----------------------------------------------------------

