
#include "cube.hpp"
#include "labyrinth.hpp"

#define REFRESH_DELAY 100


Cube cube;
Labyrinth laby(&cube);

int face=2, row=2, col=2;

void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  // Init cube
  cube.init();

  // Init laby
  uint8_t intern_walls[3] = {0b10001001, 0b00010010, 0b10010100};
  uint8_t extern_walls[2] = {0xFF, 0xFF};
  laby.init_walls(1, {4}, intern_walls, extern_walls);

  // Init hero
  laby.init_hero(4, 0, 0);
}


void loop() {
  long time = millis();

  // Trigger all the waiting callbacks
  if(!digitalRead(INT_PIN)){
    cube.read(false);
  }

  cube.show();

  time = millis() - time;
  delay(max(0, REFRESH_DELAY-time));
}
