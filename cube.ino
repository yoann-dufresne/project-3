
#include "cube.hpp"


Cube cube;


void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  cube.init();
}


void loop() {
  for (int i=0 ; i<6 ; i++)
    cube.faces[i].see_idx(i);
  cube.show();
  delay(1000);
  cube.reset_leds();
  cube.show();
  delay(1000);
}
