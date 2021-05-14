
#include "cube.hpp"


Cube cube;


void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  cube.init();

  for (int i=0 ; i<4 ; i++) {
    cube.faces[i].set_pixel(0, 0, 0, 0, 50);
    cube.faces[i].set_pixel(0, i, 0, 0, 50);
  }
  cube.faces[4].set_pixel(0, 0, 0, 0, 50);
  cube.faces[4].set_pixel(1, 0, 0, 0, 50);
  cube.faces[5].set_pixel(0, 0, 0, 0, 50);
  cube.faces[5].set_pixel(1, 1, 0, 0, 50);

  cube.show();
}


void loop() {
  delay(500);
}
