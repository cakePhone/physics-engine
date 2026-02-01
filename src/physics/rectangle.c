#include "physics/rectangle.h"

Rect rectangle_new(float h, float w) { return (Rect){h, w}; }

Rect square_new(float w) { return (Rect){w, w}; }
