#pragma once
#include <cglm/struct.h>

typedef struct {
  float radius;
} Circle;

typedef struct rectangle {
  float width;
  float height;
} Rect;

typedef enum { CircleType, RectType } ShapeType;

typedef union {
  Circle circle;
  Rect rect;
} Shape;

Circle circle_new(float radius);

Rect rectangle_new(float height, float width);

Rect square_new(float width);
