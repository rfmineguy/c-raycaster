#include "sdl_context.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

sdl_context_t sdl_initialize(int width, int height) {
  sdl_context_t ctx = {0};
  ctx.width = width;
  ctx.height = height;

  // SDL INIT
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Failed to initialize SDL [%s]\n", SDL_GetError());
    SDL_Quit();
    return ctx;
  }
  ctx.window = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!ctx.window) {
    fprintf(stderr, "Failed to create SDL window [%s]\n", SDL_GetError());
    SDL_Quit();
    ctx.result = -1;
    return ctx;
  }
  ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED);
  if (!ctx.renderer) {
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
    ctx.result = -1;
    return ctx;
  }

  return ctx;
}
void sdl_deinitialize(sdl_context_t ctx) {
  SDL_DestroyRenderer(ctx.renderer);
  SDL_DestroyWindow(ctx.window);
  SDL_Quit();
}
