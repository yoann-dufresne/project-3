#ifndef ENGINE_H
#define ENGINE_H

#include "cube.hpp"

#include "labyrinth.hpp"


class Level {
protected:
	Cube * cube;

public:
	Level(Cube * cube) {
		this->cube = cube;
	}

	virtual void init(uint8_t ** args);
	virtual bool is_over();
};


class GameEngine {
private:
	Cube * cube;
	Level * levels;

public:
	GameEngine(Cube * cube) {
		this->cube = cube;
	}

};


#endif