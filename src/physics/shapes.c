#include "physics/shapes.h"
#include "math/vecs.h"
#include <cglm/types-struct.h>
#include <raylib.h>

Circle circle_new(float radius) { return (Circle){radius}; }

Rect rectangle_new(float h, float w) { return (Rect){h, w}; }

Rect square_new(float w) { return (Rect){w, w}; }
