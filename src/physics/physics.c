#include "physics/physics.h"
#include "physics/body.h"
#include <stb/stb_ds.h>
#include <stdlib.h>
#include <sys/types.h>

PhysicsWorld world_new(void) {
  PhysicsWorld w;
  w.bodies = NULL;
  return w;
}

void world_add_body(PhysicsWorld *w, Body b) { arrput(w->bodies, b); }

int update(PhysicsWorld *w, float dt) {
  int count = arrlen(w->bodies);

  // 1. Integration Step
  for (int i = 0; i < count; i++) {
    body_integrate(&w->bodies[i], dt);
  }

  // 2. Collision Step
  for (int i = 0; i < count; i++)
    for (int j = 0; j < count; j++) {
      body_collide(&w->bodies[i], &w->bodies[j]);
    }

  // 3. Ensure no out-of-bounds
  for (int i = 0; i < count; i++)
    body_collide_bounds(&w->bodies[i]);

  return 0;
}
