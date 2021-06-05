// #include "LowPower.h"

#include "cube.hpp"
#include "gameengine.hpp"
#include "animator.hpp"
// #include "freemem.h"


Cube cube;
Animator anim(&cube);
GameEngine ge(&cube, &anim);

Level * current_level = nullptr;


void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  // Init cube
  cube.init();
}


void loop() {
  long time = millis();

  // Level update
  if (current_level == nullptr or current_level->is_over()) {
    bool is_success = true;
    if (current_level != nullptr) {
      // Remove the previous level
      is_success = current_level->is_success();
      delete current_level;
    }
    // Reset the cube leds
    cube.reset_leds();

    // Serial.print("Base mem ");Serial.println(freeMemory());
    if (is_success) {
      // Load new level
      current_level = ge.load_next_lvl();
    } else {
      // Reload level
      current_level = ge.reload_lvl();
    }
    // Serial.print("Lvl loaded mem ");Serial.println(freeMemory());
  }

  // Trigger all the waiting callbacks
  if(!digitalRead(INT_PIN)){
    cube.read(false);
  }

  // Refresh cube displays
  anim.next_cycle();
  cube.show();

  // Wait a small delay (max 10 fps to save power)
  time = millis() - time;
  delay(max(0, REFRESH_DELAY-time));
  // LowPower.idle((int)max(0, REFRESH_DELAY-time));
}
