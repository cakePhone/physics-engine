#pragma once
#include "physics/body.h"
#include <sys/types.h>

#define step 60.0f

typedef struct world {
  Body *bodies;
} PhysicsWorld;

PhysicsWorld world_new(void);

void world_add_body(PhysicsWorld *w, Body b);

int update(PhysicsWorld *w, float dt);
