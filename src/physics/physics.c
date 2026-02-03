#include "physics/physics.h"
#include "physics/body.h"
#include <stb/stb_ds.h>
#include <sys/types.h>

PhysicsWorld world_new(void) {
  PhysicsWorld w;
  w.bodies = NULL;
  return w;
}

void world_add_body(PhysicsWorld *w, Body b) { arrput(w->bodies, b); }

int update(PhysicsWorld *w, float dt) {
  int count = arrlen(w->bodies);

  int physics_steps = 16;
  dt *= 1.0f / physics_steps;
  while (physics_steps--) {
    // 1. Collision Step (resolve collisions BEFORE integration)
    for (int i = 0; i < count; i++)
      for (int j = i + 1; j < count; j++) {
        body_collide(&w->bodies[i], &w->bodies[j]);
      }

    // 2. Ensure no out-of-bounds
    for (int i = 0; i < count; i++)
      body_collide_bounds(&w->bodies[i]);

    // 3. Integration Step (apply velocities after collision resolution)
    for (int i = 0; i < count; i++) {
      body_integrate(&w->bodies[i], dt);
    }
  }

  return 0;
}
