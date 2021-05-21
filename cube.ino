
#include "cube.hpp"
#include "labyrinth.hpp"

#define REFRESH_DELAY 100


Cube cube;
Level * current_level = nullptr;

// -- For now, only one laby level
//                      nb faces and faces list
uint8_t bin_levels[10] = {1, 4,
//    Internal walls                      external walls
      0b10001001, 0b00010010, 0b10010100, 0xFF, 0xFF,
//    Hero coordinates
      4, 0, 0
// TODO: objects
};




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
    current_level = Labyrinth::lvl_from_memory(&cube, bin_levels);
  }

  // Trigger all the waiting callbacks
  if(!digitalRead(INT_PIN)){
    cube.read(false);
  }

  cube.show();

  time = millis() - time;
  delay(max(0, REFRESH_DELAY-time));
}
