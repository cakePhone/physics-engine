#include "physics/body.h"
#include "math/vecs.h"
#include "physics/collision.h"
#include <cglm/struct.h>
#include <cglm/struct/vec2.h>
#include <cglm/types-struct.h>
#include <raylib.h>
#include <stdio.h>

Body body_new(vec2s p, vec2s v, vec2s a, float m, float r, float f, Color c,
              ShapeType type, Shape shape, float angle, float av) {
  Body b;
  b.p = p;
  b.v = v;
  b.a = a;
  b.angle = angle;
  b.av = av;
  b.torque = 0.0f;
  b.m = m;
  b.im = 1.0f / m;
  b.r = r;
  b.f = f;
  b.c = c;
  b.type = type;
  b.shape = shape;

  // Calculate moment of inertia based on shape
  if (type == CircleType) {
    // I = (1/2) * m * r^2 for a solid circle
    float radius = shape.circle.radius;
    b.I = 0.5f * m * radius * radius;
  } else { // RectType
    // I = (1/12) * m * (w^2 + h^2) for a solid rectangle
    float w = shape.rect.width;
    float h = shape.rect.height;
    b.I = (1.0f / 12.0f) * m * (w * w + h * h);
  }
  
  b.iI = (b.I > 0) ? 1.0f / b.I : 0.0f;

  return b;
}

char *vec2s_to_str(vec2s v) {
  char *s = malloc(sizeof(char) * 64);
  snprintf(s, 64, "{x=%.2f,y=%.2f}", v.x, v.y);
  return s;
}

void body_integrate(Body *b, float dt) {
  // Linear integration
  b->p = glms_vec2_add(b->p, glms_vec2_scale(b->v, dt));
  b->v = glms_vec2_add(b->v, glms_vec2_scale(b->a, dt));
  
  // Angular integration
  b->angle += b->av * dt;
  b->av += b->torque * b->iI * dt;
  
  // Angular damping to prevent excessive spinning
  const float angular_damping = 0.98f;
  b->av *= angular_damping;
  
  // Clear accumulated torque
  b->torque = 0.0f;
}

char *body_to_str(Body *b) {
  char *s = malloc(sizeof(char) * 256);
  char *pos = vec2s_to_str(b->p);
  char *vel = vec2s_to_str(b->v);
  char *acc = vec2s_to_str(b->a);

  snprintf(s, 256, "Body: Pos %s; Vel %s; Acc %s;", pos, vel, acc);

  free(pos);
  free(vel);
  free(acc);

  return s;
}

void body_draw(Body *b) {
  switch (b->type) {
  case CircleType: {
    DrawCircleV(to_rl(b->p), b->shape.circle.radius, b->c);
    break;
  }
  case RectType: {
    // Convert angle from radians to degrees for Raylib
    float angle_deg = b->angle * (180.0f / 3.14159265f);
    
    // Calculate the origin (center of rectangle for rotation)
    Vector2 origin = {b->shape.rect.width / 2.0f, b->shape.rect.height / 2.0f};
    
    // DrawRectanglePro takes position at top-left, but we want to rotate around center
    Rectangle rect = {
      b->p.x,
      b->p.y,
      b->shape.rect.width,
      b->shape.rect.height
    };
    
    DrawRectanglePro(rect, origin, angle_deg, b->c);
    break;
  }
  }
}

void body_collide(Body *a, Body *b) {
  switch (a->type) {
  case CircleType:
    switch (b->type) {
    case CircleType:
      circle_collide_circle(a, b);
      break;
    case RectType:
      circle_collide_rect(a, b);
      break;
    }
    break;  // Missing break was causing fallthrough!
  case RectType:
    switch (b->type) {
    case CircleType:
      circle_collide_rect(b, a);
      break;

    case RectType:
      rect_collide_rect(a, b);
      break;
    }
  }
}

void body_collide_bounds(Body *b) {
  float width = (float)GetScreenWidth();
  float height = (float)GetScreenHeight();
  float radius = b->shape.circle.radius;

  // Horizontal Borders (Left/Right)
  if (b->p.x - radius < 0) {
    b->p.x = radius; // Snap back
    b->v.x *= -b->r; // Bounce + Restitution
  } else if (b->p.x + radius > width) {
    b->p.x = width - radius; // Snap back
    b->v.x *= -b->r;
  }

  // Vertical Borders (Top/Bottom)
  if (b->p.y - radius < 0) {
    b->p.y = radius; // Snap back
    b->v.y *= -b->r;
  } else if (b->p.y + radius > height) {
    b->p.y = height - radius; // Snap back
    b->v.y *= -b->r;
  }
}
