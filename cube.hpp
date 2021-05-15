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
  
  // Set one pixel to the rgb color
  void set_pixel(int row, int col, int r, int g, int b);
  // Get the rgb colors of one pixel
  void get_pixel(int row, int col, int & r, int & g, int & b);
  // Add the rgb colors to the current colors of a pixel. ie: for each color max between the current value and the value added.
  void add_pixel_color(int row, int col, int & r, int & g, int & b);
  // Rm the rgb colors to the current colors of a pixel. ie: for each color min between the current value and the value added.
  void rm_pixel_color(int row, int col, int & r, int & g, int & b);

  void show();
  void reset_leds();
  void see_idx(int idx);
};


class Cube {
public:
	Face faces[6];

  Cube();
  void init();

  void show();
  void reset_leds();

  /** Given the coordinates and direction, return the next coordinates.
  * The values are stored in the input variables.
  *
  * @param face face index [min: 0 ; max: 5] (1st coordinate)
  * @param row row index [min: 0 ; max: 3] (2nd coordinate)
  * @param col column index [min: 0 ; max: 3] (3rd coordinate)
  * @param direction direction index [min: 0 ; max: 3]. 0 mean north, 1 west, 2 south and 3 east.
  */
	void Cube::next_tile(int & face, int & row, int & col, int direction);
};

  
#endif
