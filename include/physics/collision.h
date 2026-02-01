#pragma once
#include "physics/body.h"
#include <cglm/struct/vec2.h>

void resolve_impulse(Body *a, Body *b, vec2s normal, float penetration, vec2s contact_point);

void circle_collide_circle(Body *a, Body *b);

void circle_collide_rect(Body *circle, Body *rect);

void rect_collide_rect(Body *a, Body *b);
