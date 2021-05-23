#ifndef ENGINE_H
#define ENGINE_H

#include "cube.hpp"


class Level {
public:
	Cube * cube;

	Level(Cube * cube) {
		this->cube = cube;
	}
	virtual ~Level() {};

	virtual bool is_over();
	virtual bool is_success();
};


class GameEngine {
private:
	Cube * cube;
	uint32_t nb_lvl;
	uint32_t program_pointer;
	uint32_t current_lvl_start;
	
public:
	GameEngine(Cube * cube);

	Level * reload_lvl();
	Level * load_next_lvl();

};


#endif