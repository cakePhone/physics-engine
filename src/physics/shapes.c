#include "physics/shapes.h"
#include "math/vecs.h"
#include "physics/body.h"
#include <cglm/types-struct.h>
#include <raylib.h>

Circle circle_new(float radius) { return (Circle){radius}; }

Triangle triangle_new(vec2s a, vec2s b, vec2s c) {
  return (Triangle){{a, b, c}};
}

Rect rectangle_new(float h, float w) { return (Rect){h, w}; }

Rect square_new(float w) { return (Rect){w, w}; }

// =============
// | Collision |
// =============

void resolve_impulse(Body *a, Body *b, vec2s normal, float penetration,
                     vec2s contact_point) {
  const float slop = 0.005f;

  if (penetration > slop) {
    // Use 100% correction - fully separate objects
    float correction_magnitude = (penetration - slop) / (a->im + b->im);
    vec2s correction = glms_vec2_scale(normal, correction_magnitude);

    // Move them apart proportional to their inverse mass
    a->p = glms_vec2_sub(a->p, glms_vec2_scale(correction, a->im));
    b->p = glms_vec2_add(b->p, glms_vec2_scale(correction, b->im));
  }

  // Calculate vectors from centers to contact point
  vec2s ra = glms_vec2_sub(contact_point, a->p);
  vec2s rb = glms_vec2_sub(contact_point, b->p);

  // Calculate relative velocity at contact point (including rotation)
  vec2s ra_perp = {-ra.y, ra.x};
  vec2s rb_perp = {-rb.y, rb.x};

  vec2s va_contact = glms_vec2_add(a->v, glms_vec2_scale(ra_perp, a->av));
  vec2s vb_contact = glms_vec2_add(b->v, glms_vec2_scale(rb_perp, b->av));
  vec2s rel_v = glms_vec2_sub(vb_contact, va_contact);

  // Check if they are actually moving toward each other
  float vel_along_normal = glms_vec2_dot(rel_v, normal);

  // If they're moving apart, don't apply impulse
  if (vel_along_normal > 0)
    return;

  // Calculate restitution
  float e = fminf(a->r, b->r);

  // Damping for low-velocity collisions to prevent jitter
  if (fabsf(vel_along_normal) < 0.05f)
    e = 0.0f;

  // Calculate impulse scalar with rotation
  float ra_cross_n = ra.x * normal.y - ra.y * normal.x;
  float rb_cross_n = rb.x * normal.y - rb.y * normal.x;

  float denom = a->im + b->im + ra_cross_n * ra_cross_n * a->iI +
                rb_cross_n * rb_cross_n * b->iI;
  float j = -(1 + e) * vel_along_normal / denom;

  // Apply linear impulse
  vec2s impulse = glms_vec2_scale(normal, j);
  a->v = glms_vec2_sub(a->v, glms_vec2_scale(impulse, a->im));
  b->v = glms_vec2_add(b->v, glms_vec2_scale(impulse, b->im));

  // Apply angular impulse
  a->av -= ra_cross_n * j * a->iI;
  b->av += rb_cross_n * j * b->iI;

  // === FRICTION ===
  vec2s tangent = {-normal.y, normal.x};

  // Recalculate relative velocity after impulse
  va_contact = glms_vec2_add(a->v, glms_vec2_scale(ra_perp, a->av));
  vb_contact = glms_vec2_add(b->v, glms_vec2_scale(rb_perp, b->av));
  rel_v = glms_vec2_sub(vb_contact, va_contact);

  float vel_along_tangent = glms_vec2_dot(rel_v, tangent);

  float friction = (a->f + b->f) * 0.5f;
  float ra_cross_t = ra.x * tangent.y - ra.y * tangent.x;
  float rb_cross_t = rb.x * tangent.y - rb.y * tangent.x;

  float denom_t = a->im + b->im + ra_cross_t * ra_cross_t * a->iI +
                  rb_cross_t * rb_cross_t * b->iI;
  float jt = -vel_along_tangent / denom_t;

  // Clamp friction
  float max_friction = fabsf(j) * friction;
  if (fabsf(jt) > max_friction) {
    jt = (jt > 0 ? 1.0f : -1.0f) * max_friction;
  }

  // Apply friction impulse
  vec2s friction_impulse = glms_vec2_scale(tangent, jt);
  a->v = glms_vec2_sub(a->v, glms_vec2_scale(friction_impulse, a->im));
  b->v = glms_vec2_add(b->v, glms_vec2_scale(friction_impulse, b->im));

  a->av -= ra_cross_t * jt * a->iI;
  b->av += rb_cross_t * jt * b->iI;
}

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

  // 3. Calculate penetration depth
  float penetration = r_sum - d;

  // 4. Calculate contact point (on the surface between the two circles)
  vec2s contact_point =
      glms_vec2_add(a->p, glms_vec2_scale(normal, a->shape.circle.radius));

  // 5. Resolve the collision using the shared impulse function
  resolve_impulse(a, b, normal, penetration, contact_point);
}

void circle_collide_rect(Body *circle_body, Body *rect_body) {
  Circle c = circle_body->shape.circle;
  Rect rect = rect_body->shape.rect;

  // Get rotation angle
  float angle = rect_body->angle;
  float cos_a = cosf(angle);
  float sin_a = sinf(angle);

  // Transform circle center into rectangle's local space
  vec2s diff = glms_vec2_sub(circle_body->p, rect_body->p);

  // Rotate the difference vector by -angle to align with rectangle's local axes
  vec2s local_circle_pos = {diff.x * cos_a + diff.y * sin_a,
                            -diff.x * sin_a + diff.y * cos_a};

  // Now work in local space where rectangle is axis-aligned
  float half_w = rect.width / 2.0f;
  float half_h = rect.height / 2.0f;

  // Find closest point on rectangle to circle center (in local space)
  float closest_x = fmaxf(-half_w, fminf(local_circle_pos.x, half_w));
  float closest_y = fmaxf(-half_h, fminf(local_circle_pos.y, half_h));
  vec2s closest_local = {closest_x, closest_y};

  // Check if circle center is inside rectangle
  int inside = (fabsf(local_circle_pos.x) < half_w) &&
               (fabsf(local_circle_pos.y) < half_h);

  vec2s local_normal;
  float penetration;

  if (inside) {
    // Circle center is inside rectangle - find closest edge to push out
    float dist_to_left = local_circle_pos.x + half_w;
    float dist_to_right = half_w - local_circle_pos.x;
    float dist_to_top = local_circle_pos.y + half_h;
    float dist_to_bottom = half_h - local_circle_pos.y;

    float min_dist = fminf(fminf(dist_to_left, dist_to_right),
                           fminf(dist_to_top, dist_to_bottom));

    if (min_dist == dist_to_left) {
      local_normal = (vec2s){{-1, 0}};
      closest_local = (vec2s){{-half_w, local_circle_pos.y}};
    } else if (min_dist == dist_to_right) {
      local_normal = (vec2s){{1, 0}};
      closest_local = (vec2s){{half_w, local_circle_pos.y}};
    } else if (min_dist == dist_to_top) {
      local_normal = (vec2s){{0, -1}};
      closest_local = (vec2s){{local_circle_pos.x, -half_h}};
    } else {
      local_normal = (vec2s){{0, 1}};
      closest_local = (vec2s){{local_circle_pos.x, half_h}};
    }

    // Penetration is distance to edge + radius
    penetration = min_dist + c.radius;
  } else {
    // Circle center is outside rectangle
    vec2s local_diff = glms_vec2_sub(local_circle_pos, closest_local);
    float distance_squared = glms_vec2_dot(local_diff, local_diff);
    float radius_squared = c.radius * c.radius;

    // No collision if distance > radius
    if (distance_squared >= radius_squared) {
      return;
    }

    float distance = sqrtf(distance_squared);

    if (distance < 0.0001f) {
      // Edge case: circle center exactly on rectangle edge
      local_normal = (vec2s){{0, -1}};
    } else {
      local_normal = glms_vec2_divs(local_diff, distance);
    }

    penetration = c.radius - distance;
  }

  // Transform normal back to world space by rotating by +angle
  // Note: local_normal points from rect toward circle, but we need circle
  // toward rect
  vec2s normal = {-(local_normal.x * cos_a - local_normal.y * sin_a),
                  -(local_normal.x * sin_a + local_normal.y * cos_a)};

  // Transform closest point to world space for contact point
  vec2s contact_world = {closest_local.x * cos_a - closest_local.y * sin_a,
                         closest_local.x * sin_a + closest_local.y * cos_a};
  vec2s contact_point = glms_vec2_add(rect_body->p, contact_world);

  resolve_impulse(circle_body, rect_body, normal, penetration, contact_point);
}

void rect_collide_rect(Body *a, Body *b) {
  Rect *ra = &a->shape.rect;
  Rect *rb = &b->shape.rect;

  // For OBB collision, we need to use Separating Axis Theorem (SAT)
  // We test 4 potential separating axes: the normals of both rectangles

  float a_cos = cosf(a->angle);
  float a_sin = sinf(a->angle);
  float b_cos = cosf(b->angle);
  float b_sin = sinf(b->angle);

  // Half extents
  float a_half_w = ra->width / 2.0f;
  float a_half_h = ra->height / 2.0f;
  float b_half_w = rb->width / 2.0f;
  float b_half_h = rb->height / 2.0f;

  // Get corners of both rectangles in world space
  vec2s a_axes[2] = {
      {a_cos, a_sin}, // A's X axis
      {-a_sin, a_cos} // A's Y axis
  };

  vec2s b_axes[2] = {
      {b_cos, b_sin}, // B's X axis
      {-b_sin, b_cos} // B's Y axis
  };

  vec2s diff = glms_vec2_sub(b->p, a->p);

  float min_overlap = FLT_MAX;
  vec2s collision_normal = {{0, 0}};

  // Test A's axes
  for (int i = 0; i < 2; i++) {
    vec2s axis = a_axes[i];

    // Project rectangles onto axis
    float a_proj = fabsf(a_half_w * glms_vec2_dot(axis, a_axes[0])) +
                   fabsf(a_half_h * glms_vec2_dot(axis, a_axes[1]));

    float b_proj = fabsf(b_half_w * glms_vec2_dot(axis, b_axes[0])) +
                   fabsf(b_half_h * glms_vec2_dot(axis, b_axes[1]));

    // Project center distance onto axis
    float dist = glms_vec2_dot(diff, axis);

    // Calculate overlap
    float overlap = a_proj + b_proj - fabsf(dist);

    if (overlap <= 0) {
      // Found separating axis - no collision
      return;
    }

    // Track minimum overlap and corresponding axis
    if (overlap < min_overlap) {
      min_overlap = overlap;
      // Ensure normal points from A to B
      collision_normal = (dist < 0) ? glms_vec2_negate(axis) : axis;
    }
  }

  // Test B's axes
  for (int i = 0; i < 2; i++) {
    vec2s axis = b_axes[i];

    // Project rectangles onto axis
    float a_proj = fabsf(a_half_w * glms_vec2_dot(axis, a_axes[0])) +
                   fabsf(a_half_h * glms_vec2_dot(axis, a_axes[1]));

    float b_proj = fabsf(b_half_w * glms_vec2_dot(axis, b_axes[0])) +
                   fabsf(b_half_h * glms_vec2_dot(axis, b_axes[1]));

    // Project center distance onto axis
    float dist = glms_vec2_dot(diff, axis);

    // Calculate overlap
    float overlap = a_proj + b_proj - fabsf(dist);

    if (overlap <= 0) {
      // Found separating axis - no collision
      return;
    }

    // Track minimum overlap and corresponding axis
    if (overlap < min_overlap) {
      min_overlap = overlap;
      // Ensure normal points from A to B
      collision_normal = (dist < 0) ? glms_vec2_negate(axis) : axis;
    }
  }

  // All axes overlapped - collision detected
  // Calculate contact point as midpoint between centers, pushed along normal
  vec2s contact_point =
      glms_vec2_add(glms_vec2_add(a->p, b->p),
                    glms_vec2_scale(collision_normal, -min_overlap * 0.5f));
  contact_point = glms_vec2_scale(contact_point, 0.5f);

  resolve_impulse(a, b, collision_normal, min_overlap, contact_point);
}
