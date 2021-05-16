
#include "cube.hpp"
#include "labyrinth.hpp"


Cube cube;

int face=2, row=2, col=2;

void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  // Init cube
  cube.init();
  cube.reset_leds();

  // Init laby
  Labyrinth laby(cube);
  uint8_t intern_walls[3] = {0b10001001, 0b00010010, 0b10010100};
  uint8_t extern_walls[2] = {0xFF, 0xFF};
  laby.init_walls(1, {3}, intern_walls, extern_walls);

  // show first pixel
  cube.faces[face].add_pixel_color(row, col, 0, 50, 0);
  cube.faces[face].show();
}


void loop() {
  // Trigger all the waiting callbacks
  if(!digitalRead(INT_PIN)){
    cube.read(false);
  }

  delay(50);
}
