#include <stdint.h>
#include <math.h>

#include "cube.hpp"

#define PI 3.14159265


void rot(int & row, int & col, int rotation) {
	int r2=row, c2=col;
	
	switch(rotation) {
		case 1:
			r2 = 3 - col;
			c2 = row;
			break;
		case 2:
			r2 = 3 - row;
			c2 = 3 - col;
			break;
		case 3:
			r2 = col;
			c2 = 3 - row;
	}

	row = r2;
	col = c2;

	// Serial.print(row); Serial.print(" "); Serial.print(col); Serial.print(" "); Serial.println(rotation);
	// Serial.print(r2); Serial.print(" "); Serial.println(c2); Serial.println();
}


Face::Face() {
	this->is_init = false;
}


void Face::init(int addr, int rotation) {
	Serial.print("Face init ");
	Serial.println(addr);

	this->rotation = rotation;

  this->trellis = new Adafruit_NeoTrellis(addr);
  if (!this->trellis->begin(addr)) {
    this->is_init = false;
  } else {
    Serial.println("NeoPixel Trellis started");
    this->is_init = true;
  }

}

void Face::set_pixel(int row, int col, int r, int g, int b) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);

	this->trellis->pixels.setPixelColor(
		row * 4 + col,
		this->trellis->pixels.Color(r, g, b)
	);
}

void Face::show() {
	if (not is_init)
		return;

	this->trellis->pixels.show();
	// this->changed = false;
}



Cube::Cube() {

}

void Cube::init() {
	Serial.println("Cube init");
	int addrs[6] = {0x2E, 0x2F, 0x30, 0x32, 0x36, 0x3E};
	int rotations[6] = {2, 0, 3, 3, 1, 1};
	for (int i=0 ; i<6 ; i++) {
		this->faces[i].init(addrs[i], rotations[i]);
	}
}

void Cube::show() {
	for (int i=0 ; i<6 ; i++) {
		delay(10);
		this->faces[i].show();
		delay(10);
	}
}
