#include <stdio.h>
#include <limits.h>
#include <SDL.h>
#include "ray.h"
#include "boundary.h"
#include "sdl_context.h"

#define BOUNDARY_COUNT 7

void ray_game_mouse(sdl_context_t ctx, boundary_t boundaries[BOUNDARY_COUNT]) {
  static ray_t ray = (ray_t) {};

  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  int dx = 0, dy = 0;
  if (keystate[SDL_SCANCODE_W]) dy -= 1;
  if (keystate[SDL_SCANCODE_S]) dy += 1;
  if (keystate[SDL_SCANCODE_A]) dx -= 1;
  if (keystate[SDL_SCANCODE_D]) dx += 1;
  ray_move(&ray, dx, dy, 0);

  // Update/Draw the ray
  int mousex, mousey;
  SDL_GetMouseState(&mousex, &mousey);
  ray_update_xy(&ray, mousex, mousey, 10);
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

  // Draw the boundaries
  for (int i = 0; i < BOUNDARY_COUNT; i++) {
    boundary_t boundary = boundaries[i];
    SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x00, 0x00, 0xff);
    if (has_hit && i == closest) {
      SDL_SetRenderDrawColor(ctx.renderer, 0x00, 0xff, 0x00, 0xff);
    }
    SDL_RenderDrawLine(ctx.renderer, boundary.x1, boundary.y1, boundary.x2, boundary.y2);
  }
}

void ray_game_angle(sdl_context_t ctx, boundary_t boundaries[BOUNDARY_COUNT]) {
#define RAY_COUNT 7
  static ray_t rays[RAY_COUNT] = {0};
  const Uint8* keystate = SDL_GetKeyboardState(NULL);

  // Movement
  int dx = 0, dy = 0;
  float dtheta_rad = 0;
  if (keystate[SDL_SCANCODE_W]) dy -= 1;
  if (keystate[SDL_SCANCODE_S]) dy += 1;
  if (keystate[SDL_SCANCODE_A]) dx -= 1;
  if (keystate[SDL_SCANCODE_D]) dx += 1;
  if (keystate[SDL_SCANCODE_Q]) dtheta_rad += 0.01f;
  if (keystate[SDL_SCANCODE_E]) dtheta_rad -= 0.01f;

  for (int i = 0; i < RAY_COUNT; i++) {
    double degrees = (180.0 / RAY_COUNT) * i;
    printf("%f\n", degrees);
    double rads = degrees * (M_PI / 180.0);
    ray_update_angle(&rays[i], rads, 100);
    ray_move(&rays[i], dx, dy, dtheta_rad);
    ray_draw(rays[i], ctx);
  }

  for (int i = 0; i < RAY_COUNT; i++) {
    int closest_boundary = -1;
    int closest_boundary_dist = INT_MAX;
    for (int j = 0; j < BOUNDARY_COUNT; j++) {
      boundary_t* b = &boundaries[j];
      int hit_x, hit_y;
      if (ray_cast_against(rays[i], *b, &hit_x, &hit_y)) {
        int hit_distance = (int)((hit_x-rays[i].x1) * (hit_x-rays[i].x1) + (hit_y-rays[i].y1) *(hit_y-rays[i].y1));
        if (hit_distance < closest_boundary_dist) {
          boundaries[closest_boundary].hit = 0;
          closest_boundary_dist = hit_distance;
          closest_boundary = j;
          boundaries[closest_boundary].hit = 0;
          SDL_Rect r = (SDL_Rect) {.x=hit_x,.y=hit_y,.w=10,.h=10};
          SDL_RenderDrawRect(ctx.renderer, &r);
          b->hit = 1;
        }
      }
    }
  }

  for (int i = 0; i < BOUNDARY_COUNT; i++) {
    boundary_t b = boundaries[i];
    if (b.hit) {
      SDL_SetRenderDrawColor(ctx.renderer, 0x00, 0xff, 0x00, 0xff);
    }
    else {
      SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x00, 0x00, 0xff);
    }
    SDL_RenderDrawLine(ctx.renderer, b.x1, b.y1, b.x2, b.y2);
    boundaries[i].hit = 0;
  }
}

int main() {
  sdl_context_t ctx = sdl_initialize(600, 600);
  if (ctx.result < 0) {
    sdl_deinitialize(ctx);
    return ctx.result;
  }

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

    // ray_game_mouse(ctx, boundaries);
    ray_game_angle(ctx, boundaries);

    SDL_RenderPresent(ctx.renderer);
  }

  sdl_deinitialize(ctx);
  return 0;
}
