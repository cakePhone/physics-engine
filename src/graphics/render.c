#include "physics/physics.h"
#include <raylib.h>
#include <stb/stb_ds.h>
#include <sys/types.h>

int render(PhysicsWorld w) {
  BeginDrawing();
  ClearBackground(BLACK);

  int body_count = arrlen(w.bodies);
  for (uint i = 0; i < body_count; i++) {
    body_draw(&w.bodies[i]);
  }

  EndDrawing();
  return 0;
}
