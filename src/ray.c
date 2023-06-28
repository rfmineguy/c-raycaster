#include "ray.h"

int ray_cast_against(ray_t ray, boundary_t boundary, int* hit_x, int* hit_y) {
  float x1 = ray.x1, x2 = ray.x2, x3 = boundary.x1, x4 = boundary.x2;
  float y1 = ray.y1, y2 = ray.y2, y3 = boundary.y1, y4 = boundary.y2;

  float t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
  float t_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float t = t_num / t_den;

  float u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
  float u_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float u = u_num / u_den;

  *hit_x = x1 + t * (x2 - x1);
  *hit_y = y1 + t * (y2 - y1);

  return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}

void ray_update_angle(ray_t* ray, float angle, int length) {
  int target_x = ray->x1 + (cos(ray->angle_rad + angle) * length);
  int target_y = ray->y1 + (sin(ray->angle_rad + angle) * length);
  ray->x2 = target_x;
  ray->y2 = target_y;
}

void ray_update_xy(ray_t* ray, int mousex, int mousey, int length) {
  int dx = mousex - ray->x1;
  int dy = mousey - ray->y1;
  ray->x2 = mousex;
  ray->y2 = mousey;
}

void ray_move(ray_t* ray, int dx, int dy, float dtheta_rad) {
  ray->x1 += dx;
  ray->y1 += dy;
  ray->angle_rad += dtheta_rad;
}

void ray_draw(ray_t ray, sdl_context_t ctx) {
  SDL_Rect rect = (SDL_Rect) {.x=ray.x1, .y=ray.y1, .w=10, .h=10};
  
  SDL_SetRenderDrawColor(ctx.renderer, 0x15, 0x15, 0x15, 0xff);
  SDL_RenderFillRect(ctx.renderer, &rect);

  SDL_SetRenderDrawColor(ctx.renderer, 0xff, 0x7f, 0x00, 0xff);
  SDL_RenderDrawLine(ctx.renderer, ray.x1, ray.y1, ray.x2, ray.y2);
}
