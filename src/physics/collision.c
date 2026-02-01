#include "physics/body.h"
#include "physics/circle.h"
#include <cglm/struct.h>
#include <cglm/struct/vec2.h>

void circle_collide_circle(Body *a, Body *b) {
  if (a == b)
    return;

  // 1. Distance check
  vec2s collision_axis = glms_vec2_sub(b->p, a->p); // Vector A -> B
  float d = glms_vec2_norm(collision_axis);
  float r_sum = a->shape.circle.radius + b->shape.circle.radius;

  if (d >= r_sum || d == 0.0f)
    return;

  // 2. Normal A -> B
  vec2s normal = glms_vec2_divs(collision_axis, d);

  // 3. Relative Velocity
  vec2s rel_v = glms_vec2_sub(b->v, a->v);

  // 4. Check if they are actually moving toward each other
  float vel_along_normal = glms_vec2_dot(rel_v, normal);
  if (vel_along_normal > 0)
    return;

  // 5. Calculate Impulse Scalar (j)
  float e = fminf(a->r, b->r); // Use restitution
  float j = -(1 + e) * vel_along_normal;
  j /= (a->im + b->im);

  // 6. Apply the Impulse along the Normal
  vec2s impulse = glms_vec2_scale(normal, j);

  // Body A moves away from B, Body B moves away from A
  a->v = glms_vec2_sub(a->v, glms_vec2_scale(impulse, a->im));
  b->v = glms_vec2_add(b->v, glms_vec2_scale(impulse, b->im));

  const float percent = 0.4f; // How much of the overlap to fix (0.2 to 0.8)
  const float slop = 0.01f;   // Allowed overlap before correction kicks in

  // Calculate how much they are overlapping
  float penetration = r_sum - d;

  if (penetration > slop) {
    vec2s correction =
        glms_vec2_scale(normal, (penetration / (a->im + b->im)) * percent);

    // Move them apart proportional to their inverse mass
    a->p = glms_vec2_sub(a->p, glms_vec2_scale(correction, a->im));
    b->p = glms_vec2_add(b->p, glms_vec2_scale(correction, b->im));
  }
}
