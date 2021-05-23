#include <string.h>

#include "labyrinth.hpp"


Labyrinth::~Labyrinth() {
	for (int f=0 ; f<6 ; f++)
		for (int r=0 ; r<4 ; r++)
			for (int c=0 ; c<4 ; c++)
				if (this->objects[f][r][c] != nullptr) {
					delete this->objects[f][r][c];
				}
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

uint8_t Labyrinth::get_walls(Coordinates & coords) {
	uint8_t walls = 0;

  // North
  if (coords.row == 0) {
    walls |= (this->extern_walls[coords.face * 2 + 1] >> coords.col) & 0b1;
  } else {
    int wall_idx = 12 + 3 * coords.col + coords.row - 1;
    int byte_idx = wall_idx / 8;
    walls |= (this->intern_walls[coords.face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1;
  }

  // West
  if (coords.col == 0) {
    walls |= ((this->extern_walls[coords.face * 2 + 0] >> (7 - coords.row)) & 0b1) << 1;
  } else {
    int wall_idx = 3 * coords.row + coords.col - 1;
    int byte_idx = wall_idx / 8;
    walls |= ((this->intern_walls[coords.face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 1;
  }

  // South
  if (coords.row == 3) {
    walls |= ((this->extern_walls[coords.face * 2 + 0] >> (3 - coords.col)) & 0b1) << 2;
  } else {
    int wall_idx = 12 + 3 * coords.col + coords.row;
    int byte_idx = wall_idx / 8;
    walls |= ((this->intern_walls[coords.face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 2;
  }

  // East
  if (coords.col == 3) {
    walls |= ((this->extern_walls[coords.face * 2 + 1] >> (4 + coords.row)) & 0b1) << 3;
  } else {
    int wall_idx = 3 * coords.row + coords.col;
    int byte_idx = wall_idx / 8;
    walls |= ((this->intern_walls[coords.face * 3 + byte_idx] >> (7 - (wall_idx % 8))) & 0b1) << 3;
  }

  return walls;
}


// ----- Hero -----

void Labyrinth::init_hero(Coordinates & coords) {
	this->hero = Coordinates(coords);

	if (this->hero.row == 0) {
		this->hero.row = 1;
		uint8_t direction = 0;
		this->hero_move(this->hero, &direction);
	} else {
		this->hero.row -= 1;
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


void Labyrinth::hero_move(Coordinates & coords, uint8_t * args) {
	// Serial.println("Move");
	uint8_t direction = args[0];

	// Remove previous button interactions
	uint8_t walls = this->get_walls(coords);
	for (uint8_t i=0 ; i<4 ; i++) {
		// If there is a wall, nothing to do
		if ((walls >> i) & 0b1)
			continue;

		// Get the button coordinates
		Coordinates button_coords(coords);
		Coordinates::next_coord(button_coords, i);

		// Unset the button callback
		this->cube->faces[button_coords.face].unbind_btn_callback(button_coords.row, button_coords.col);
		this->cube->faces[button_coords.face].deactivate_btn(
				button_coords.row,
				button_coords.col,
				SEESAW_KEYPAD_EDGE_RISING
		);

		// Remove coridor color
		this->cube->faces[button_coords.face].rm_pixel_color(
			button_coords.row, button_coords.col,
			255, 255, 0);
	}
	
	// Remove the hero from previous tile
	this->cube->faces[coords.face].set_pixel(coords.row, coords.col, 0, 0, 0);
	if (this->objects[coords.face][coords.row][coords.col] != nullptr)
		this->objects[coords.face][coords.row][coords.col]->set_colors(*this);

	// Move in memory to next tile
	Coordinates::next_coord(coords, direction);

	// Show in new position
	this->cube->faces[coords.face].set_pixel(coords.row, coords.col, 50, 50, 50);

	// Activate object if present
	if (this->objects[coords.face][coords.row][coords.col] != nullptr)
		this->objects[coords.face][coords.row][coords.col]->activate(*this);

	if (this->completed)
		return;

	// Decide move buttons
	walls = this->get_walls(coords);
	for (uint8_t i=0 ; i<4 ; i++) {
		// If there is a wall, nothing to do
		if ((walls >> i) & 0b1) {
			continue;
		}

		// Get the button coordinates
		Coordinates button_coords(coords);
		Coordinates::next_coord(button_coords, i);

		// Remove coridor color
		this->cube->faces[button_coords.face].add_pixel_color(button_coords.row, button_coords.col, 0, 0, 30);

		// Set the button callback
    current_laby = this;
    this->cube->faces[button_coords.face].activate_btn(
    	button_coords.row, button_coords.col, SEESAW_KEYPAD_EDGE_RISING);
    this->cube->faces[button_coords.face].bind_btn_callback(
    	button_coords.row, button_coords.col, callbacks[i]);
	}
}


// ----- Other objects -----


void Labyrinth::init_object(LabObject * lo) {
	this->objects[lo->coordinates.face][lo->coordinates.row][lo->coordinates.col] = lo;
	lo->set_colors(*this);
}


void WinPoint::set_colors(Labyrinth & laby) {
	// Serial.println("Set colors");
	laby.cube->faces[this->coordinates.face].add_pixel_color(
		this->coordinates.row, this->coordinates.col,
		0, 40, 0
	);
}

// Win animation
void WinPoint::activate(Labyrinth & laby) {
	Cube * cube = laby.cube;

	laby.completed = true;
	laby.win = true;

	delay(500);
}

void Enemy::set_colors(Labyrinth & laby) {
	// Serial.println("Set colors");
	laby.cube->faces[this->coordinates.face].add_pixel_color(
		this->coordinates.row, this->coordinates.col,
		40, 0, 0
	);
}

// Win animation
void Enemy::activate(Labyrinth & laby) {
	Cube * cube = laby.cube;

	laby.completed = true;
	laby.win = false;
}
