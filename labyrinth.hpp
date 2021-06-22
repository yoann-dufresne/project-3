#ifndef LABY_H
#define LABY_H


#include <stdint.h>

#include "cube.hpp"
#include "gameengine.hpp"
#include "progreader.hpp"
#include "animator.hpp"

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

class Teleporter : public LabObject {
public:
	Coordinates target;
	Teleporter(Coordinates & source, Coordinates & target) : LabObject(source) {
		this->target = Coordinates(target);
	};
	
	void set_colors(Labyrinth & laby);
	void activate(Labyrinth & laby);
};



class Labyrinth : public Level {
private:
	// uint8_t nb_faces;
	// uint8_t faces[6];
	uint8_t intern_walls[3*6];
	uint8_t extern_walls[2*6];

public:
	LabObject ** obj_list;
	uint8_t next_free;
	uint8_t obj_refs[6][4][4];

	Coordinates hero;
	Animation * hero_anim;
	bool completed;
	bool win;

	Labyrinth(Cube * cube, Animator * anim) : Level(cube, anim) {
		for (int f=0 ; f<6 ; f++)
			for (int r=0 ; r<4 ; r++)
				for (int c=0 ; c<4 ; c++)
					this->obj_refs[f][r][c] = 255;

		this->completed = false;
		this->win = false;
		this->hero_anim = nullptr;

		this->next_free = 0;
	}

	~Labyrinth();

	bool is_over() {return this->completed;};
	bool is_success() {return this->win;};

	/** Declare the used faces and their walls.
	  * 
	  * @param face The face index to update.
	  * @param intern_walls 3 bytes per face used where each bit represent a wall.
	  * The first 12 bits correspond to the 12 vertical walls from top left to bottom right,
	  * row by row. The 12 nexts bits are the vertical walls olumn by column.
	  * The patern repeat for each face used.
	  * @param extern_walls 2 bytes per face used, representing the 16 possible walls surronding
	  * a face. From corner top left, the walls are in the same order than the trigonometric
	  * direction (counter clockwise).
	  **/
	void init_walls(uint8_t face, uint8_t * walls);
	uint8_t get_walls(Coordinates & coords);

	void hero_remove();
	void hero_add(Coordinates & coordinates);
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
	static Level * lvl_from_memory(Cube * cube, Animator * anim, uint32_t & pp) {
		// Serial.print("Laby size ");Serial.println(sizeof(Labyrinth));
		// Serial.print("Enemy size ");Serial.println(sizeof(Enemy));

		Labyrinth * laby = new Labyrinth(cube, anim);

	 	// Init faces and walls
	 	uint8_t used_faces = prog(pp++);
	 	uint8_t hero_compact_coords = prog(pp++);
	 	// Serial.print(used_faces);Serial.print(" ");Serial.println(hero_compact_coords);delay(10);

	 	uint8_t walls[5];
	 	for (int f=0 ; f<6 ; f++) {
	 		if (not (used_faces >> f & 0b1))
	 			continue;

			// Load from memory the 5 walls bytes
			for (int w=0 ; w<5 ; w++)
				walls[w] = prog(pp++);

			laby->init_walls(f, walls);
		}

		// Serial.println("Walls ok");delay(10);

		// Lab objects
		uint8_t nb_objects = prog(pp++);
		laby->set_nb_objects(nb_objects);
		// Serial.print("nb objs: ");Serial.println(nb_objects);delay(10);
		for (int i=0 ; i<nb_objects ; i++) {
			uint8_t obj_type = prog(pp++);
			uint8_t obj_compact_coords = prog(pp++);
			// Serial.print("obj coords ");Serial.println(obj_compact_coords);delay(10);
			Coordinates obj_coords(obj_compact_coords);

			LabObject * lo;
			switch(obj_type) {
			case 'w':
				lo = new WinPoint(obj_coords);
				break;

			case 'e':
				lo = new Enemy(obj_coords);
				break;

			case 't':
				uint8_t compact_target = prog(pp++);
				Coordinates target(compact_target);
				lo = new Teleporter(obj_coords, target);
				break;
			}

			laby->init_object(lo);
		}

		// Init hero
		Coordinates hero_coords(hero_compact_coords);
		laby->hero_add(hero_coords);
		return laby;
	}
};




#endif