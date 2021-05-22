#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>

#include "Arduino.h"
#include "Adafruit_NeoTrellis.h"

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
  void set_pixel(int row, int col, int r, int g, int b);
  // Get the rgb colors of one pixel
  void get_pixel(int row, int col, int & r, int & g, int & b);
  // Add the rgb colors to the current colors of a pixel. ie: for each color max between the current value and the value added.
  void add_pixel_color(int row, int col, int r, int g, int b);
  // Rm the rgb colors to the current colors of a pixel. ie: for each color min between the current value and the value added.
  void rm_pixel_color(int row, int col, int r, int g, int b);

  void show();
  void reset_leds();
  void see_idx(int idx);

  void activate_btn(int row, int col, uint8_t event);
  void deactivate_btn(int row, int col, uint8_t event);
  void bind_btn_callback(int row, int col, TrellisCallback cb);
  void unbind_btn_callback(int row, int col);
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
  uint8_t face;
  uint8_t row;
  uint8_t col;

  Coordinates() {
    this->face = 6;
    this->row = 4;
    this->col = 4;
  }

  Coordinates(uint8_t face, uint8_t row, uint8_t col) {
    this->face = face;
    this->row = row;
    this->col = col;
  }

  Coordinates(uint8_t * coords) {
    this->face = coords[0];
    this->row = coords[1];
    this->col = coords[2];
  }

  Coordinates(Coordinates & c) {
    this->face = c.face;
    this->row = c.row;
    this->col = c.col;
  }

  /** Given the coordinates and direction, return the next coordinates.
  * The values are stored in the input variables.
  *
  * @param face face index [min: 0 ; max: 5] (1st coordinate)
  * @param row row index [min: 0 ; max: 3] (2nd coordinate)
  * @param col column index [min: 0 ; max: 3] (3rd coordinate)
  * @param direction direction index [min: 0 ; max: 3]. 0 means north, 1 west, 2 south and 3 east.
  */
  static void next_coord(Coordinates & coord, int direction) {
    switch (direction) {
    case 0:
      coord.row -= 1;

      // Overflow
      if (coord.row == -1) {
        coord.face = 3 * (coord.face / 3) + ((coord.face + 1) % 3);
        coord.row = coord.col;
        coord.col = 0;
      }
      break;

    case 1:
      coord.col -= 1;
      
      // Overflow
      if (coord.col == -1) {
        coord.face = 3 * (coord.face / 3) + ((coord.face + 2) % 3);
        coord.col = coord.row;
        coord.row = 0;
      }
      break;

    case 2:
      coord.row += 1;

      // Overflow
      if (coord.row == 4) {
        //     Face triplet change      +   opposite face
        coord.face = ((coord.face / 3 + 1) % 2) * 3 + 2 - (coord.face + 1) % 3;
        coord.col = 3 - coord.col;
        coord.row = 3;
      }
      break;

    case 3:
      coord.col += 1;

      // Overflow
      if (coord.col == 4) {
        //     Face triplet change      +   opposite face
        coord.face = ((coord.face / 3 + 1) % 2) * 3 + 2 - (coord.face + 2) % 3;
        coord.col = 3;
        coord.row = 3 - coord.row;
      }
      break;
    }
  }
};

  
#endif
