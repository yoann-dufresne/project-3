#ifndef CUBE_H
#define CUBE_H

#include "Arduino.h"
#include "Adafruit_NeoTrellis.h"


class Face {
private:
	bool is_init;
	int rotation;
	Adafruit_NeoTrellis * trellis;
public:
	Face();
  void init(int addr, int rotation);
  void set_pixel(int row, int col, int r, int g, int b);
  void show();
};


class Cube {
public:
	Face faces[6];

  Cube();
  void init();
  void show();
};

  
#endif
