#pragma once
#include "physics/circle.h"
#include <cglm/types-struct.h>
#include <raylib.h>

typedef enum { CircleType } ShapeType;

typedef union {
  Circle circle;
} Shape;

typedef struct body {
  // Motion
  vec2s p; // position
  vec2s v; // velocity
  vec2s a; // acceleration

  // Physical Constants
  float m;  // mass
  float im; // inverted mass: pre-calculated 1.0f / mass
  float r;  // restitution
  float f;  // friction

  // Visuals/Metadata
  Color c; // color

  ShapeType type;
  Shape shape;
} Body;

Body body_new(vec2s position, vec2s velocity, vec2s acceleration, float mass,
              float restitution, float friction, Color color, ShapeType shape,
              Shape shape_data);

void body_integrate(Body *b, float dt);

void body_collide(Body *a, Body *b);

void body_collide_bounds(Body *b);

void body_draw(Body *b);
