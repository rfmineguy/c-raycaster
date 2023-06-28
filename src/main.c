#include <stdio.h>
#include <limits.h>
#include <SDL.h>
#include "ray.h"
#include "boundary.h"
#include "sdl_context.h"

int main() {
  sdl_context_t ctx = sdl_initialize(600, 600);
  if (ctx.result < 0) {
    sdl_deinitialize(ctx);
    return ctx.result;
  }

  const int BOUNDARY_COUNT = 7;
  ray_t ray = (ray_t) {.x1=300, .y1=300};
  boundary_t boundaries[BOUNDARY_COUNT] = {0};
  boundaries[0] = (boundary_t) {.x1=130, .y1=90, .x2=130, .y2=400};
  boundaries[1] = (boundary_t) {.x1=150, .y1=90, .x2=150, .y2=400};
  boundaries[2] = (boundary_t) {.x1=130, .y1=90, .x2=190, .y2=40};
  boundaries[3] = (boundary_t) {.x1=130, .y1=40, .x2=190, .y2=90};
  boundaries[4] = (boundary_t) {.x1=230, .y1=50, .x2=320, .y2=90};
  boundaries[5] = (boundary_t) {.x1=430, .y1=340, .x2=190, .y2=90};
  boundaries[6] = (boundary_t) {.x1=30,  .y1=190, .x2=135, .y2=125};

  // GAME LOOP
  int running = 1;
  SDL_Event e;
  while (running) {
    if (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT: running = 0; break;
      }
    }
    SDL_SetRenderDrawColor(ctx.renderer, 0x55, 0x55, 0x55, 0xff);
    SDL_RenderClear(ctx.renderer);

    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_w) {
        ray.y1 -= 5;
      }
      if (e.key.keysym.sym == SDLK_s) {
        ray.y1 += 5;
      }
      if (e.key.keysym.sym == SDLK_a) {
        ray.x1 -= 5;
      }
      if (e.key.keysym.sym == SDLK_d) {
        ray.x1 += 5;
      }
    }

    // Update/Draw the ray
    int mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    ray_update(&ray, mousex, mousey, 10);
    ray_draw(ray, ctx);

    // Figure out which boundaries were hit and which one was the closest
    int closest = 0, has_hit = 0;
    int intersection_distances[BOUNDARY_COUNT] = {INT_MAX};
    for (int i = 0; i < BOUNDARY_COUNT; i++) {
      boundary_t boundary = boundaries[i];
      int hit_x, hit_y;
      if (ray_cast_against(ray, boundary, &hit_x, &hit_y)) {
        SDL_Rect rect = {.x=hit_x, .y=hit_y, .w=15, .h=15};
        SDL_RenderFillRect(ctx.renderer, &rect);
        intersection_distances[i] = (int)((hit_x-ray.x1) * (hit_x-ray.x1) + (hit_y-ray.y1) * (hit_y-ray.y1));

        if (intersection_distances[i] < intersection_distances[closest]) {
          closest = i;
        }
        has_hit = 1;
      }
    }

    /*
    for (int i = 0; i < 360; i += 60) {
      int hit_x, hit_y;
      if (ray_cast_against(ray, i, boundary, &hit_x, &hit_y)) {
        ...
      }
    }
    */

    // Draw the boundaries
    for (int i = 0; i < BOUNDARY_COUNT; i++) {
      boundary_t boundary = boundaries[i];
      SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x00, 0x00, 0xff);
      if (has_hit && i == closest) {
        SDL_SetRenderDrawColor(ctx.renderer, 0x00, 0xff, 0x00, 0xff);
      }
      SDL_RenderDrawLine(ctx.renderer, boundary.x1, boundary.y1, boundary.x2, boundary.y2);
    }
    
    SDL_RenderPresent(ctx.renderer);
  }

  sdl_deinitialize(ctx);
  return 0;
}
