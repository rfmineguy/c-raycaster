#ifndef RAY_H
#define RAY_H
#include <SDL.h>
#include "boundary.h"
#include "sdl_context.h"

typedef struct {
  int x1, y1;
  int x2, y2;
} ray_t;

int ray_cast_against(ray_t, boundary_t, int*, int*);
void ray_update(ray_t*, int, int, int);
void ray_move(ray_t*, int, int);
void ray_draw(ray_t, sdl_context_t);

#endif
