#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <SDL.h>
#include <stdio.h>
#include "ray.h"
#include "boundary.h"
#include "sdl_context.h"

#define BOUNDARY_COUNT 7
#define RAY_COUNT 7

int random_range_i(int lower, int upper) {
  int random = rand() % (upper - lower);
  return random + lower;
}

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

void ray_game_angle(sdl_context_t ctx, ray_t rays[RAY_COUNT], boundary_t boundaries[BOUNDARY_COUNT]) {
  printf("Ray Game Angle\n");
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

  // Setup rays
  for (int i = 0; i < RAY_COUNT; i++) {
    double degrees = (180.0 / RAY_COUNT) * i;
    printf("%f\n", degrees);
    double rads = degrees * (M_PI / 180.0);
    ray_update_angle(&rays[i], rads, 100);
    ray_move(&rays[i], dx, dy, dtheta_rad);
    ray_draw(rays[i], ctx);
  }

  // Perform raycasting
  int closest_boundaries[RAY_COUNT] = {-1};
  int closest_distances[RAY_COUNT] = {INT_MAX};
  for (int i = 0; i < RAY_COUNT; i++) {
    printf("Raycasting\n");
    int closest_hit_x = 0, closest_hit_y = 0;
    int closest_boundary_dist = INT_MAX;
    for (int j = 0; j < BOUNDARY_COUNT; j++) {
      boundary_t* b = &boundaries[j];
      int hit_x, hit_y;
      if (ray_cast_against(rays[i], *b, &hit_x, &hit_y)) {
        int hit_distance = (int)((hit_x-rays[i].x1) * (hit_x-rays[i].x1) + (hit_y-rays[i].y1) *(hit_y-rays[i].y1));
        if (hit_distance < closest_boundary_dist) {
          closest_boundaries[i] = j;
          closest_boundary_dist = hit_distance;
          closest_distances[i] = closest_boundary_dist;
          closest_hit_x = hit_x;
          closest_hit_y = hit_y;
        }
      }
    }
    printf("Done Raycasting\n");

    printf("Rendering...\n");
    SDL_Rect rect = (SDL_Rect) {.x=closest_hit_x, .y=closest_hit_y, .w=10, .h=10};
    SDL_RenderDrawRect(ctx.renderer, &rect);
  }

  // Visualize hits
  for (int i = 0; i < RAY_COUNT; i++) {
    int closest_boundary = closest_boundaries[i];
    if (closest_boundary == -1) {
      
    }
    boundary_t b = boundaries[closest_boundaries[i]];
    if (closest_boundary == i && b.hit && closest_boundary >= 0) {
      SDL_SetRenderDrawColor(ctx.renderer, 0x00, 0xff, 0x00, 0xff);
    }
    else {
      SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x00, 0x00, 0xff);
    }
    SDL_RenderDrawLine(ctx.renderer, b.x1, b.y1, b.x2, b.y2);
    /*
    if (closest_boundary > 0 && b.hit && i == closest_boundary) {
      SDL_SetRenderDrawColor(ctx.renderer, 0x00, 0xff, 0x00, 0xff);
    }
    else {
      SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x00, 0x00, 0xff);
    }
    SDL_RenderDrawLine(ctx.renderer, b.x1, b.y1, b.x2, b.y2);
    */
  }

  for (int i = 0; i < RAY_COUNT; i++) {
    rays[i].has_hit = 0;
  }
}

int main() {
  fprintf(stderr, "MASSIVE MEMORY LEAK\n");
  return 342;

  sdl_context_t ctx = sdl_initialize(600, 600);
  if (ctx.result < 0) {
    sdl_deinitialize(ctx);
    return ctx.result;
  }

  // RANDOMIZE BOUNDARIES
  boundary_t boundaries[BOUNDARY_COUNT] = {0};
  srand(time(NULL));
  for (int i = 0; i < BOUNDARY_COUNT; i++) {
    boundaries[i].x1 = random_range_i(0, 600);
    boundaries[i].x2 = random_range_i(0, 600);
    boundaries[i].y1 = random_range_i(0, 600);
    boundaries[i].y2 = random_range_i(0, 600);
  }

  ray_t rays[RAY_COUNT] = {0};
  for (int i = 0; i < RAY_COUNT; i++) {
    double degrees = (180.0 / RAY_COUNT) * i;
    printf("%f\n", degrees);
    double rads = degrees * (M_PI / 180.0);
    ray_update_angle(&rays[i], rads, 100);
  }

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
    ray_game_angle(ctx, rays, boundaries);

    SDL_RenderPresent(ctx.renderer);
  }

  sdl_deinitialize(ctx);
  return 0;
}
