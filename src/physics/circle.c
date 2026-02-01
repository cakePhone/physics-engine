#include "physics/circle.h"
#include <cglm/struct.h>
#include <cglm/struct/vec2.h>
#include <cglm/types-struct.h>
#include <math/vecs.h>
#include <raylib.h>
#include <stdbool.h>

Circle circle_new(float radius) { return (Circle){radius}; }
