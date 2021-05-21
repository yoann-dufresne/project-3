#include <string.h>

#include "labyrinth.hpp"


void Labyrinth::init(uint8_t ** args) {

}

// ----- Walls -----

void Labyrinth::init_walls(uint8_t nb_faces, uint8_t * faces, uint8_t * walls) {
	this->nb_faces = nb_faces;
	memcpy(this->faces, faces, nb_faces);

	for (uint8_t n=0 ; n<nb_faces ; n++) {
		uint8_t face_idx = this->faces[n];
		memcpy(this->intern_walls + 3 * face_idx, walls + 5 * n, 3);
		memcpy(this->extern_walls + 2 * face_idx, walls + 5 * n + 3, 2);
	}
}

uint8_t Labyrinth::get_walls(uint8_t face, uint8_t row, uint8_t col) {
	uint8_t walls = 0;

	// North
	if (row == 0) {
		walls |= (this->extern_walls[face * 2 + 1] >> col) & 0b1;
	} else {
		int wall_idx = 12 + 3 * col + row - 1;
		int byte_idx = wall_idx / 8;
		walls |= (this->intern_walls[face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1;
	}

	// West
	if (col == 0) {
		walls |= ((this->extern_walls[face * 2 + 0] >> (7 - row)) & 0b1) << 1;
	} else {
		int wall_idx = 3 * row + col - 1;
		int byte_idx = wall_idx / 8;
		walls |= ((this->intern_walls[face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 1;
	}

	// South
	if (row == 3) {
		walls |= ((this->extern_walls[face * 2 + 0] >> (3 - col)) & 0b1) << 2;
	} else {
		int wall_idx = 12 + 3 * col + row;
		int byte_idx = wall_idx / 8;
		walls |= ((this->intern_walls[face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 2;
	}

	// East
	if (col == 3) {
		walls |= ((this->extern_walls[face * 2 + 1] >> (4 + row)) & 0b1) << 3;
	} else {
		int wall_idx = 3 * row + col;
		int byte_idx = wall_idx / 8;
		walls |= ((this->intern_walls[face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 3;
	}

	return walls;
}


// ----- Hero -----

void Labyrinth::init_hero(uint8_t face, uint8_t row, uint8_t col) {
	this->hero[0] = face;
	this->hero[1] = row;
	this->hero[2] = col;

	if (row == 0) {
		this->hero[1] = 1;
		uint8_t direction = 0;
		this->hero_move(this->hero, &direction);
	} else {
		this->hero[1] -= 1;
		uint8_t direction = 2;
		this->hero_move(this->hero, &direction);
	}
}



Labyrinth * current_laby;

void hero_mv_north(keyEvent evt) {
	uint8_t direction = 0;
	current_laby->hero_move(current_laby->hero, &direction);
}
void hero_mv_west(keyEvent evt) {
	uint8_t direction = 1;
	current_laby->hero_move(current_laby->hero, &direction);
}
void hero_mv_south(keyEvent evt) {
	uint8_t direction = 2;
	current_laby->hero_move(current_laby->hero, &direction);
}
void hero_mv_east(keyEvent evt) {
	uint8_t direction = 3;
	current_laby->hero_move(current_laby->hero, &direction);
}

void (*callbacks[4])(keyEvent) = {hero_mv_north, hero_mv_west, hero_mv_south, hero_mv_east};


void Labyrinth::hero_move(uint8_t * coordinates, uint8_t * args) {
	// Serial.println("Move");
	int face = coordinates[0];
	int row = coordinates[1];
	int col = coordinates[2];
	uint8_t direction = args[0];

	// Remove previous button interactions
	uint8_t walls = this->get_walls(face, row, col);
	for (uint8_t i=0 ; i<4 ; i++) {
		// If there is a wall, nothing to do
		if ((walls >> i) & 0b1)
			continue;

		// Get the button coordinates
		int button_face = face;
		int button_row = row;
		int button_col = col;
		this->cube->next_tile(button_face, button_row, button_col, i);

		// Unset the button callback
		this->cube->faces[button_face].deactivate_btn(button_row, button_col, SEESAW_KEYPAD_EDGE_RISING);
		this->cube->faces[button_face].unbind_btn_callback(button_row, button_col);

		// Remove coridor color
		this->cube->faces[button_face].rm_pixel_color(button_row, button_col, 255, 255, 0);
	}
	
	// Remove the hero from previous tile
	this->cube->faces[face].set_pixel(row, col, 0, 0, 0);
	// TODO: update potential objects on this tile

	// Move in memory to next tile
	this->cube->next_tile(face, row, col, direction);
	coordinates[0] = face;
	coordinates[1] = row;
	coordinates[2] = col;

	// Show in new position
	this->cube->faces[face].set_pixel(row, col, 50, 50, 10);

	// Activate object if present
	if (this->objects[face][row][col] != nullptr)
		this->objects[face][row][col]->activate(*this);

	// Decide move buttons
	walls = this->get_walls(face, row, col);
	for (uint8_t i=0 ; i<4 ; i++) {
		// If there is a wall, nothing to do
		if ((walls >> i) & 0b1) {
			continue;
		}

		// Get the button coordinates
		int button_face = face;
		int button_row = row;
		int button_col = col;
		this->cube->next_tile(button_face, button_row, button_col, i);

		// Remove coridor color
		this->cube->faces[button_face].add_pixel_color(button_row, button_col, 0, 0, 30);

		// Set the button callback
    current_laby = this;
    this->cube->faces[button_face].activate_btn(
    	button_row, button_col, SEESAW_KEYPAD_EDGE_RISING);
    this->cube->faces[button_face].bind_btn_callback(button_row, button_col, callbacks[i]);
	}
}


// ----- Other objects -----


void Labyrinth::init_object(LabObject * lo) {
	this->objects[lo->coordinates[0]][lo->coordinates[1]][lo->coordinates[2]] = lo;
}
