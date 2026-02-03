#pragma once
#include <cglm/struct.h>

// forwards declare body so no recursive include
typedef struct body Body;

typedef struct {
  float radius;
} Circle;

typedef struct triangle {
  vec2s points[3];
} Triangle;

typedef struct rectangle {
  float width;
  float height;
} Rect;

#define SHAPE_COUNT 3
typedef enum { CircleType, TriangleType, RectType } ShapeType;

typedef union {
  Circle circle;
  Triangle tri;
  Rect rect;
} Shape;

Circle circle_new(float radius);

Triangle triangle_new(vec2s a, vec2s b, vec2s c);

Rect rectangle_new(float height, float width);

Rect square_new(float width);

// =============
// | Collision |
// =============

void resolve_impulse(Body *a, Body *b, vec2s normal, float penetration,
                     vec2s contact_point);

void circle_collide_circle(Body *a, Body *b);

void circle_collide_rect(Body *circle, Body *rect);

void rect_collide_rect(Body *a, Body *b);
