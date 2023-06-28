#ifndef SDL_CONTEXT_H
#define SDL_CONTEXT_H
#include <SDL.h>

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int result;
  int width, height;
  float lastFrame, currentFrame;
  float dt;
} sdl_context_t;

sdl_context_t sdl_initialize(int, int);
void sdl_deinitialize(sdl_context_t ctx);

#endif
