#ifndef BOUNDARY_H
#define BOUNDARY_H
#include <SDL.h>

typedef struct {
  int x1, y1;
  int x2, y2;
  int hit, hit_by;
} boundary_t;

#endif
