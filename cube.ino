
#include "cube.hpp"


Cube cube;
int current_pixels[6];


void setup() {
  // Begin serial
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  cube.init();

  for (int i=0 ; i<6 ; i++) {
    int n = random(0, 16);
    current_pixels[i] = n;
    cube.faces[i].set_pixel(n/4, n%4, 50, 50, 50);
  }

  cube.show();
}


void loop() {
  int t_idx = random(0, 6);
  Serial.print("Board ");
  Serial.println(t_idx);
  // Turn off previous led
  int n = current_pixels[t_idx];
  cube.faces[t_idx].set_pixel(n/4, n%4, 0, 0, 0);
  // change pixel
  n = random(0, 16);
  current_pixels[t_idx] = n;
  // Turn on new led
  cube.faces[t_idx].set_pixel(n/4, n%4, 50, 50, 50);
  // Update trellis
  cube.faces[t_idx].show();

  delay(500);
}
