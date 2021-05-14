#include <stdint.h>

#include "cube.hpp"


Face::Face() {
	this->is_init = false;
}


void Face::init(int addr, int rotation) {
	Serial.print("Face init ");
	Serial.println(addr);

	// this->changed = false;

  this->trellis = new Adafruit_NeoTrellis(addr);
  if (!this->trellis->begin(addr)) {
    Serial.println("Could not start trellis, check wiring?");
    this->is_init = false;
  } else {
    Serial.println("NeoPixel Trellis started");
    this->is_init = true;
  }

}

void Face::set_pixel(int row, int col, int r, int g, int b) {
	if (not is_init)
		return;

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
	for (int i=0 ; i<6 ; i++) {
		this->faces[i].init(addrs[i], 0);
	}
}

void Cube::show() {
	for (int i=0 ; i<6 ; i++) {
		Serial.print("\t");
		Serial.println(i);
		delay(10);
		this->faces[i].show();
		Serial.println("\tDone");
		delay(10);
	}
}
