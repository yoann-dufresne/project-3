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
	int rp, gp, bp;
	this->get_pixel(row, col, rp, gp, bp);
	this->set_pixel(row, col, max(r, rp), max(g, gp), max(b, bp));
}

void Face::rm_pixel_color(int row, int col, int r, int g, int b) {
	int rp, gp, bp;
	this->get_pixel(row, col, rp, gp, bp);
	this->set_pixel(row, col, min(r, rp), min(g, gp), min(b, bp));
}

void Face::show() {
	if (not is_init)
		return;

	this->trellis->pixels.show();
	// this->changed = false;
}

void Face::reset_leds() {
	for (int r=0 ; r<4 ; r++)
		for (int c=0 ; c<4 ; c++)
			this->trellis->pixels.setPixelColor(r * 4 + c, 0);
}

void Face::see_idx(int idx) {
	this->set_pixel(0, 0, 0, 0, 50);
	this->set_pixel(idx/4, idx%4, 0, 0, 50);
}

void Face::activate_btn(int row, int col, uint8_t event) {
	this->trellis->activateKey(row * 4 + col, event);
}

void Face::deactivate_btn(int row, int col, uint8_t event) {
	this->trellis->activateKey(row * 4 + col, event, false);
}

void Face::bind_btn_callback(int row, int col, TrellisCallback cb) {
	this->trellis->registerCallback(row * 4 + col, cb);
}

void Face::unbind_btn_callback(int row, int col) {
	this->trellis->unregisterCallback(row * 4 + col);
}




Cube::Cube() {}

void Cube::init() {
	Serial.println("Cube init");
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
		delay(10);
		this->faces[i].show();
		delay(10);
	}
}

void Cube::reset_leds() {
	for (int i=0 ; i<6 ; i++)
		this->faces[i].reset_leds();
}

/** Given the coordinates and direction, return the next coordinates.
  * The values are stored in the input variables.
  *
  * @param face face index [min: 0 ; max: 5] (1st coordinate)
  * @param row row index [min: 0 ; max: 3] (2nd coordinate)
  * @param col column index [min: 0 ; max: 3] (3rd coordinate)
  * @param direction direction index [min: 0 ; max: 3]. 0 mean north, 1 west, 2 south and 3 east.
  */
void Cube::next_tile(int & face, int & row, int & col, int direction) {
	switch (direction) {
	case 0:
		row -= 1;

		// Overflow
		if (row == -1) {
			face = 3 * (face / 3) + ((face + 1) % 3);
			row = col;
			col = 0;
		}
		break;

	case 1:
		col -= 1;
		
		// Overflow
		if (col == -1) {
			face = 3 * (face / 3) + ((face + 2) % 3);
			col = row;
			row = 0;
		}
		break;

	case 2:
		row += 1;

		// Overflow
		if (row == 4) {
			//     Face triplet change      +   opposite face
			face = ((face / 3 + 1) % 2) * 3 + 2 - (face + 1) % 3;
			col = 3 - col;
			row = 3;
		}
		break;

	case 3:
		col += 1;

		// Overflow
		if (col == 4) {
			//     Face triplet change      +   opposite face
			face = ((face / 3 + 1) % 2) * 3 + 2 - (face + 2) % 3;
			col = 3;
			row = 3 - row;
		}
		break;
	}
}

