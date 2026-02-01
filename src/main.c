#include "graphics/render.h"
#include "physics/body.h"
#include "physics/physics.h"
#include "physics/rectangle.h"
#include <cglm/types-struct.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

vec2s rand_vec2s_range(vec2s clampMin, vec2s clampMax) {
  float x = clampMin.x +
            ((float)rand() / (float)RAND_MAX) * (clampMax.x - clampMin.x);
  float y = clampMin.y +
            ((float)rand() / (float)RAND_MAX) * (clampMax.y - clampMin.y);
  return (vec2s){x, y};
}

float rand_range(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

Color rand_color() {
  int r = rand_range(0, 255);
  int g = rand_range(0, 255);
  int b = rand_range(0, 255);
  return (Color){r, g, b, 255};
}

int main(void) {
  srand(time(NULL));
  const int width = 800;
  const int height = 600;

  vec2s minPos = {0, 0};
  vec2s maxPos = {width, height};

  vec2s minVel = {-100, -100};
  vec2s maxVel = {100, 100};

  PhysicsWorld world = world_new();

  for (int i = 100; i > 0; i--) {
    vec2s pos = rand_vec2s_range(minPos, maxPos);
    vec2s vel = rand_vec2s_range(minVel, maxVel);
    vec2s acc = rand_vec2s_range((vec2s){-100, 1000}, (vec2s){100, 1000});

    Shape s;
    ShapeType t;
    if (i % 3 == 0) {
      s = (Shape){.rect =
                      rectangle_new(rand_range(10, 30), rand_range(10, 30))};
      t = RectType;
    } else {
      s = (Shape){.circle = circle_new(rand_range(5, 15))};
      t = CircleType;
    }

    Body b = body_new(pos, vel, acc, rand_range(10, 100), rand_range(0.6, 1),
                      rand_range(0.2, 0.5), rand_color(), t, s,
                      rand_range(0, 6.28f), rand_range(-1.0f, 1.0f));

    world_add_body(&world, b);
  }

  InitWindow(width, height, "2D Physics");

  SetTargetFPS(144);

  const float fixed_dt = 1.0f / 144.0f;
  float accumulator = 0.0f;

  while (!WindowShouldClose()) {
    float frameTime = GetFrameTime();
    accumulator += frameTime;

    // clamp to avoid spiral of death
    const float max_accum = 0.25f;
    if (accumulator > max_accum)
      accumulator = max_accum;

    while (accumulator >= fixed_dt) {
      update(&world, fixed_dt);
      accumulator -= fixed_dt;
    }

    render(world);
  }

  CloseWindow();

  return 0;
}
