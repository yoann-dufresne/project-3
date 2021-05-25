#ifndef LABY_H
#define LABY_H


#include <stdint.h>

#include "cube.hpp"
#include "gameengine.hpp"
#include "progreader.hpp"

#include "Arduino.h"


class Labyrinth;

class LabObject {
public:
	Coordinates coordinates;

	LabObject(uint8_t face, uint8_t row, uint8_t col) {
		this->coordinates = Coordinates(face, row, col);
	};
	LabObject(Coordinates & coordinates) {
		this->coordinates = Coordinates(coordinates);
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
	LabObject ** obj_list;
	uint8_t next_free;
	uint8_t obj_refs[6][4][4];

	Coordinates hero;
	bool completed;
	bool win;

	Labyrinth(Cube * cube) : Level(cube) {
		for (int f=0 ; f<6 ; f++)
			for (int r=0 ; r<4 ; r++)
				for (int c=0 ; c<4 ; c++)
					this->obj_refs[f][r][c] = 255;

		this->completed = false;
		this->win = false;

		this->next_free = 0;
	}

	~Labyrinth();

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

	void set_nb_objects(uint8_t nb_objects);
	void init_object(LabObject * lo);

	/** Labyrinth binary representation is as follow:
	  * - 1 byte for number of faces
	  * - 1 byte per face used for face definition
	  * - 5 bytes per face used for walls (3 for intern walls + 2 for externs)
	  * - 3 bytes for hero start
	  * - 1 byte for num of object used in the labyrinth
	  * - 4 bytes per object in the lab (1 for obj type + 3 for coordinates)
	  **/
	static Level * lvl_from_memory(Cube * cube, uint32_t & pp) {
		// Serial.print("Laby size ");Serial.println(sizeof(Labyrinth));
		// Serial.print("Enemy size ");Serial.println(sizeof(Enemy));

		Labyrinth * laby = new Labyrinth(cube);

	 	// Init faces and walls
	 	uint8_t nb_faces = prog(pp);
	 	pp += 1;
	 	uint8_t faces[6];
	 	uint8_t walls[30];
	 	for (int f=0 ; f<nb_faces ; f++) {
	 		// Load faces names from memory
			faces[f] = prog(pp + f);
			// Load from memory the 5 walls bytes
			for (int w=0 ; w<5 ; w++) {
				walls[w] = prog(pp + nb_faces + f * 5 + w);
			}
		}

		laby->init_walls(nb_faces, faces, walls);
		pp += 6 * nb_faces;

		// Init hero
		uint8_t * coords_uint = faces;
		for (int c=0 ; c<3 ; c++)
			coords_uint[c] = prog(pp++);
		Coordinates coords(coords_uint);
		laby->init_hero(coords);

		// Lab objects
		uint8_t nb_objects = prog(pp++);
		laby->set_nb_objects(nb_objects);

		for (int i=0 ; i<nb_objects ; i++) {
			uint8_t obj_type = prog(pp++);

			Coordinates obj_coords;
			LabObject * lo;
			switch(obj_type) {
			case 'w':
				for (int c=0 ; c<3 ; c++)
					coords_uint[c] = prog(pp++);
				obj_coords = Coordinates(coords_uint);
				lo = new WinPoint(obj_coords);
				break;

			case 'l':
				for (int c=0 ; c<3 ; c++)
					coords_uint[c] = prog(pp++);
				obj_coords = Coordinates(coords_uint);
				lo = new Enemy(obj_coords);
				break;
			}

			laby->init_object(lo);
		}

		return laby;
	}
};




#endif