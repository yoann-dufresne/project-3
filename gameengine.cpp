#include "gameengine.hpp"
#include "labyrinth.hpp";
#include "progreader.hpp"




GameEngine::GameEngine(Cube * cube, Animator * anim) {
	this->cube = cube;
	this->anim = anim;
	this->current_lvl_start = 1;
	this->program_pointer = 1;
	
	this->nb_lvl = prog(0);
}


Level * GameEngine::reload_lvl() {
	this->program_pointer = this->current_lvl_start;
	this->nb_lvl += 1;
	return this->load_next_lvl();
}


Level * GameEngine::load_next_lvl() {
	Serial.println("load lvl");delay(10);
	// If no lvl left
	if (this->nb_lvl == 0) {
		this->nb_lvl = prog(0);
		this->program_pointer = 1;
	}
	// Serial.println("Update");
	// Update level start
	this->current_lvl_start = this->program_pointer;

	// Serial.println("Lvl type");
	// Get the correct level type
	uint8_t lvl_type = prog(this->program_pointer);
	// Serial.println((char)lvl_type);
	this->program_pointer += 1;

	// Serial.println("Switch");
	uint32_t consumed_bytes = 0;
	Level * lvl;
	switch (lvl_type) {
	case 'L':
		lvl = Labyrinth::lvl_from_memory(
				this->cube, this->anim,
				this->program_pointer);
		break;
	}

	// Serial.println();
	this->nb_lvl -= 1;
	return lvl;
}
