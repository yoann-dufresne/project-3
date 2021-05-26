#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>

#include "Arduino.h"
#include "Adafruit_NeoTrellis.h"

#define REFRESH_DELAY 100
#define INT_PIN 10


class Face {
private:
	bool is_init;
	int rotation;
	Adafruit_NeoTrellis * trellis;

  friend class Cube;

public:
  bool refresh_needed;
  
	Face();
  void init(int addr, int rotation);
  
  // Set one pixel to the rgb color
  void set_pixel(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);
  // Get the rgb colors of one pixel
  void get_pixel(uint8_t row, uint8_t col, uint8_t & r, uint8_t & g, uint8_t & b);
  // Add the rgb colors to the current colors of a pixel. ie: for each color max between the current value and the value added.
  void add_pixel_color(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);
  // Rm the rgb colors to the current colors of a pixel. ie: for each color min between the current value and the value added.
  void rm_pixel_color(uint8_t row, uint8_t col, uint8_t r, uint8_t g, uint8_t b);

  void show();
  void reset_leds();
  void see_idx(int idx);

  void activate_btn(uint8_t row, uint8_t col, uint8_t event);
  void deactivate_btn(uint8_t row, uint8_t col, uint8_t event);
  void bind_btn_callback(uint8_t row, uint8_t col, TrellisCallback cb);
  void unbind_btn_callback(uint8_t row, uint8_t col);
};


class Cube {
public:
	Face faces[6];

  Cube();
  void init();
  void read(bool pooling);

  void show();
  void reset_leds();
};


class Coordinates {
public:
  uint8_t compact_coords;

  Coordinates() {
    this->compact_coords = 0xFF;
  }

  Coordinates(uint8_t face, uint8_t row, uint8_t col) {
    this->compact_coords = ((face & 0b111) << 4) | ((row & 0b11) << 2) | (col & 0b11);
  }

  Coordinates(uint8_t * coords) {
    this->compact_coords = ((coords[0] & 0b111) << 4) | ((coords[1] & 0b11) << 2) | (coords[2] & 0b11);
  }

  Coordinates(Coordinates & c) {
    this->compact_coords = c.compact_coords;
  }

  uint8_t face() { return this->compact_coords >> 4; }
  uint8_t row() { return (this->compact_coords >> 2) & 0b11; }
  uint8_t col() { return this->compact_coords & 0b11; }

  /** Given the coordinates and direction, return the next coordinates.
  * The values are stored in the input variables.
  *
  * @param face face index [min: 0 ; max: 5] (1st coordinate)
  * @param row row index [min: 0 ; max: 3] (2nd coordinate)
  * @param col column index [min: 0 ; max: 3] (3rd coordinate)
  * @param direction direction index [min: 0 ; max: 3]. 0 means north, 1 west, 2 south and 3 east.
  */
  static void next_coord(Coordinates & coord, int direction) {
    int8_t face = coord.face();
    int8_t row = coord.row();
    int8_t col = coord.col();

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

    coord.compact_coords = ((face & 0b111) << 4) | ((row & 0b11) << 2) | (col & 0b11);
  }
};

  
#endif
