#ifndef ENGINE_H
#define ENGINE_H

#include "cube.hpp"
#include "animator.hpp"


class Level {
public:
	Cube * cube;
	Animator * anim;

	Level(Cube * cube, Animator * anim) {
		this->cube = cube;
		this->anim = anim;
	}
	virtual ~Level() {};

	virtual bool is_over();
	virtual bool is_success();
};


class GameEngine {
private:
	Cube * cube;
	Animator * anim;
	uint32_t nb_lvl;
	uint32_t program_pointer;
	uint32_t current_lvl_start;
	
public:
	GameEngine(Cube * cube, Animator * anim);

	Level * reload_lvl();
	Level * load_next_lvl();

};


#endif