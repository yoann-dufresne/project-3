#include <stdint.h>

#include "cube.hpp"
#include "Arduino.h"


#ifndef LABY_H
#define LABY_H

class Labyrinth {
private:
	Cube * cube;

	uint8_t nb_faces;
	uint8_t * faces;
	uint8_t * intern_walls;
	uint8_t * extern_walls;

public:
	uint8_t hero[3];

	Labyrinth(Cube * cube) {
		this->cube = cube;
	}

	/** Declare the used faces and their walls.
	  * 
	  * @param nb_faces number of faces used in this labyrinthe.
	  * @param faces Index of used faces.
	  * @param intern_walls 3 bytes per face used where each bit represent a wall.
	  * The first 12 bits correspond to the 12 vertical walls from top left to bottom right,
	  * row by row. The 12 nexts bits are the vertical walls olumn by column.
	  * The patern repeat for each face used.
	  * @param extern_walls 2 bytes per face used, representing the 16 possible walls surronding
	  * a face. From corner top left, the walls are in the same order than the trigonometric
	  * direction (counter clockwise).
	  **/
	void init_walls(uint8_t nb_faces, uint8_t * faces, uint8_t * intern_walls, uint8_t * extern_walls);
	uint8_t get_walls(uint8_t face, uint8_t row, uint8_t col);

	void init_hero(uint8_t face, uint8_t row, uint8_t col);
	void hero_move(uint8_t * coordinates, uint8_t * args);
};

#endif