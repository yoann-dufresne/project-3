#include <math.h>

#include "cube.hpp"


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
}

void arot(int & row, int col, int rotation) {
	rot(row, col, 3-rotation);
}


Face::Face() {
	this->is_init = false;
}


void Face::init(int addr, int rotation) {
	// Serial.print("Face init ");
	// Serial.println(addr);

	this->rotation = rotation;

  this->trellis = new Adafruit_NeoTrellis(addr);
  if (!this->trellis->begin(addr)) {
    this->is_init = false;
  } else {
    // Serial.println("NeoPixel Trellis started");
    this->is_init = true;
    this->reset_leds();
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
	this->refresh_needed = true;
	// Serial.print("Set pixel ");Serial.print(row);Serial.print(" ");Serial.println(col);
}

void Face::get_pixel(int row, int col, int & r, int & g, int & b) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);

	uint32_t color = this->trellis->pixels.getPixelColor(row * 4 + col);
	b = color & 0xFF;
	g = (color >> 8) & 0xFF;
	r = (color >> 16) & 0xFF;
}

void Face::add_pixel_color(int row, int col, int r, int g, int b) {
	if (not is_init)
		return;

	int rp, gp, bp;
	this->get_pixel(row, col, rp, gp, bp);
	this->set_pixel(row, col, max(r, rp), max(g, gp), max(b, bp));
}

void Face::rm_pixel_color(int row, int col, int r, int g, int b) {
	if (not is_init)
		return;

	int rp, gp, bp;
	this->get_pixel(row, col, rp, gp, bp);
	this->set_pixel(row, col, min(r, rp), min(g, gp), min(b, bp));
}

void Face::show() {
	// Serial.print("FACE SHOW ");Serial.println(this->refresh_needed);
	if (not is_init)
		return;

	if (this->refresh_needed) {
		this->trellis->pixels.show();
		this->refresh_needed = false;
	}
}

void Face::reset_leds() {
	for (int r=0 ; r<4 ; r++)
		for (int c=0 ; c<4 ; c++)
			this->set_pixel(r, c, 0, 0, 0);
}

void Face::see_idx(int idx) {
	this->set_pixel(0, 0, 0, 0, 50);
	this->set_pixel(idx/4, idx%4, 0, 0, 50);
}

void Face::activate_btn(int row, int col, uint8_t event) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);

	this->trellis->activateKey(row * 4 + col, event);
}

void Face::deactivate_btn(int row, int col, uint8_t event) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);
	
	this->trellis->activateKey(row * 4 + col, event, false);
}

void Face::bind_btn_callback(int row, int col, TrellisCallback cb) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);
	
	this->trellis->registerCallback(row * 4 + col, cb);
}

void Face::unbind_btn_callback(int row, int col) {
	if (not is_init)
		return;

	rot(col, row, this->rotation);
	
	this->trellis->unregisterCallback(row * 4 + col);
}




Cube::Cube() {}

void Cube::init() {
	// Init faces
	int addrs[6] = {0x2E, 0x2F, 0x30, 0x36, 0x32, 0x3E};
	int rotations[6] = {2, 0, 3, 1, 3, 1};
	for (int i=0 ; i<6 ; i++) {
		this->faces[i].init(addrs[i], rotations[i]);
	}

	// Init interuption pin
	pinMode(INT_PIN, INPUT);
}

void Cube::read(bool pooling) {
	for (int i=0 ; i<6 ; i++)
    this->faces[i].trellis->read(pooling);
}

void Cube::show() {
	for (int i=0 ; i<6 ; i++) {
		this->faces[i].show();
	}
}

void Cube::reset_leds() {
	for (int i=0 ; i<6 ; i++)
		this->faces[i].reset_leds();
}



