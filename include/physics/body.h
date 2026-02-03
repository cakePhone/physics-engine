#pragma once
#include "physics/shapes.h"
#include <cglm/types-struct.h>
#include <raylib.h>

typedef struct body {
  // Linear Motion
  vec2s p; // position
  vec2s v; // velocity
  vec2s a; // acceleration

  // Rotational Motion
  float angle;  // rotation angle in radians
  float av;     // angular velocity (radians/second)
  float torque; // accumulated torque

  // Physical Constants
  float m;  // mass
  float im; // inverted mass: pre-calculated 1.0f / mass
  float I;  // moment of inertia
  float iI; // inverted moment of inertia: pre-calculated 1.0f / I
  float r;  // restitution
  float f;  // friction

  // Visuals/Metadata
  Color c; // color

  ShapeType type;
  Shape shape;
} Body;

Body body_new(vec2s position, vec2s velocity, vec2s acceleration, float mass,
              float restitution, float friction, Color color, ShapeType shape,
              Shape shape_data, float angle, float angular_velocity);

void body_integrate(Body *b, float dt);

void body_collide(Body *a, Body *b);

void body_collide_bounds(Body *b);

void body_draw(Body *b);
