#pragma once

typedef struct rectangle {
  float width;
  float height;
} Rect;

Rect rectangle_new(float height, float width);

Rect square_new(float width);
