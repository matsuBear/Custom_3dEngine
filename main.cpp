#include "Window.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>

int main() {
  Window win("Rasterizer", 800, 600);

  float t = 0.0f;

  while (win.pollEvents()) {
    win.clear(0xFF000000);

    // Animated gradient — sanity check that pixels and timing work
    for (int y = 0; y < win.height(); y++) {
      for (int x = 0; x < win.width(); x++) {
        uint8_t r = static_cast<uint8_t>((float)x / win.width() * 255);
        uint8_t g = static_cast<uint8_t>(std::abs(std::cos(t)) * 255);
        uint8_t b = static_cast<uint8_t>(std::abs(std::sin(t)) * 255);
        uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
        win.setPixel(x, y, color);
      }
    }

    // Keyboard example: hold W to shift blue channel
    if (win.isKeyDown(SDL_SCANCODE_W))
      t += 0.05f;
    if (win.isKeyDown(SDL_SCANCODE_S))
      t -= 0.05f;

    win.present();
    t += 0.01f;
  }

  return 0;
}
