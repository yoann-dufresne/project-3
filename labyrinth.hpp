#ifndef LABY_H
#define LABY_H


#include <stdint.h>

#include "cube.hpp"
#include "gameengine.hpp"

#include "Arduino.h"


class Labyrinth;

class LabObject {
public:
	Coordinates coordinates;

	LabObject(uint8_t face, uint8_t row, uint8_t col) {
		this->coordinates.face = face;
		this->coordinates.row = row;
		this->coordinates.col = col;
	};
	LabObject(Coordinates & coordinates) {
		this->coordinates.face = coordinates.face;
		this->coordinates.row = coordinates.row;
		this->coordinates.col = coordinates.col;
	}
	virtual void set_colors(Labyrinth & laby);
	virtual void activate(Labyrinth & laby);
};


class WinPoint : public LabObject {
public:
	WinPoint(uint8_t face, uint8_t row, uint8_t col) : LabObject(face, row, col) {};
	WinPoint(Coordinates & coordinates) : LabObject(coordinates) {};

	void set_colors(Labyrinth & laby);
	void activate(Labyrinth & laby);
};

class Enemy : public LabObject {
public:
	Enemy(uint8_t face, uint8_t row, uint8_t col) : LabObject(face, row, col) {};
	Enemy(Coordinates & coordinates) : LabObject(coordinates) {};

	void set_colors(Labyrinth & laby);
	void activate(Labyrinth & laby);
};



class Labyrinth : public Level {
private:
	uint8_t nb_faces;
	uint8_t faces[6];
	uint8_t intern_walls[3*6];
	uint8_t extern_walls[2*6];

public:
	LabObject * objects[6][4][4];

	Coordinates hero;
	bool completed;
	bool win;

	Labyrinth(Cube * cube) : Level(cube) {
		for (int f=0 ; f<6 ; f++)
			for (int r=0 ; r<4 ; r++)
				for (int c=0 ; c<4 ; c++)
					this->objects[f][r][c] = nullptr;

		this->completed = false;
		this->win = false;
	}

	~Labyrinth() {
		for (int f=0 ; f<6 ; f++)
			for (int r=0 ; r<4 ; r++)
				for (int c=0 ; c<4 ; c++)
					if (this->objects[f][r][c] != nullptr)
						delete this->objects[f][r][c];
	}

	void init(uint8_t ** args);
	bool is_over() {return this->completed;};
	bool is_success() {return this->win;};

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
	void init_walls(uint8_t nb_faces, uint8_t * faces, uint8_t * walls);
	uint8_t get_walls(Coordinates & coords);

	void init_hero(Coordinates & coordinates);
	void hero_move(Coordinates & coordinates, uint8_t * args);

	void init_object(LabObject * lo);

	/** Labyrinth binary representation is as follow:
	  * - 1 byte for number of faces
	  * - 1 byte per face used for face definition
	  * - 5 bytes per face used for walls (3 for intern walls + 2 for externs)
	  * - 3 bytes for hero start
	  * - 1 byte for num of object used in the labyrinth
	  * - 4 bytes per object in the lab (1 for obj type + 3 for coordinates)
	  **/
	static Level * lvl_from_memory(Cube * cube, uint8_t * bin_data, uint32_t & byte_used) {
		Labyrinth * laby = new Labyrinth(cube);
	 	byte_used = 0;

	 	// Init faces and walls
	 	uint8_t nb_faces = bin_data[0];
	 	uint8_t * faces = bin_data + 1;
	 	uint8_t * walls = bin_data + 1 + nb_faces;

		laby->init_walls(nb_faces, faces, walls);
		byte_used = 1 + nb_faces + 5 * nb_faces;

		// Init hero
		Coordinates coords(bin_data + byte_used);
		laby->init_hero(coords);
		byte_used += 3;

		// Lab objects
		uint8_t nb_objects = bin_data[byte_used];
		byte_used += 1;

		for (int i=0 ; i<nb_objects ; i++) {
			uint8_t obj_type = bin_data[byte_used++];

			LabObject * lo;

			Coordinates obj_coords;
			switch(obj_type) {
			case 'w':
				obj_coords = Coordinates(bin_data+byte_used);
				lo = new WinPoint(obj_coords);
				byte_used += 3;
				break;

			case 'l':
				obj_coords = Coordinates(bin_data+byte_used);
				lo = new Enemy(obj_coords);
				byte_used += 3;
				break;
			}

			laby->init_object(lo);
		}

		return laby;
	}
};




#endif