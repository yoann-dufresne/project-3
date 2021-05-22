#include "gameengine.hpp"
#include "labyrinth.hpp";




uint8_t bin_levels[] = {
//		Nb levels
			2,

//    Laby, nb faces and faces list
			'L',  1, 4,
//    Internal walls                      external walls
      0b10001001, 0b00010010, 0b10010100, 0xFF, 0xFF,
//    Hero coordinates
      4, 0, 0,
// Objects
      2,
      'w', 4, 3, 3,
      'l', 4, 2, 2,

//    Laby, nb faces and faces list
			'L',  1, 4,
//    Internal walls                      external walls
      0b10001001, 0b00010010, 0b10010100, 0xFF, 0xFF,
//    Hero coordinates
      4, 1, 1,
// Objects
      3,
      'w', 4, 0, 3,
      'l', 4, 0, 1,
      'l', 4, 3, 3,
};



GameEngine::GameEngine(Cube * cube) {
	this->cube = cube;
	this->current_lvl_start = 1;
	this->program_pointer = 1;
	
	this->nb_lvl = bin_levels[0];
}


Level * GameEngine::reload_lvl() {
	this->program_pointer = this->current_lvl_start;
	return this->load_next_lvl();
}


Level * GameEngine::load_next_lvl() {
	// If no lvl left
	if (this->nb_lvl == 0) {
		this->nb_lvl = bin_levels[0];
		this->program_pointer = 1;
	}
	// Update level start
	this->current_lvl_start = this->program_pointer;

	// Get the correct level type
	uint8_t lvl_type = bin_levels[this->program_pointer];
	this->program_pointer += 1;

	uint32_t consumed_bytes = 0;
	Level * lvl;
	switch (lvl_type) {
	case 'L':
		lvl = Labyrinth::lvl_from_memory(
				this->cube,
				bin_levels + this->program_pointer,
				consumed_bytes); 
		break;
	}

	this->nb_lvl -= 1;
	this->program_pointer += consumed_bytes;
	return lvl;
}
