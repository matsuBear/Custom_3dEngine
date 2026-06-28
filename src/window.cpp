#include "window.hpp"
#include "VAO.hpp"
#include "float3.hpp"

Window::Window(const std::string &title, int width, int height)
    : width(width), height(height) {

  SDL_Init(SDL_INIT_VIDEO);

  m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  glContext = SDL_GL_CreateContext(m_window);

  GLenum err = glewInit();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);
  glFrontFace(GL_CW);
  glClearDepth(0.0f);

  erase();
}

Window::~Window() {
  SDL_DestroyWindow(m_window);
  SDL_GL_DeleteContext(glContext);
}

void Window::present() { SDL_GL_SwapWindow(m_window); }

void Window::erase() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
