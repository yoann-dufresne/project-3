#include "animator.hpp"



Animator::Animator(Cube * cube) {
	this->cube = cube;
	this->cycle = 0;
	this->animation = nullptr;
}

void Animator::add_animation(Animation * animation) {
	animation->next_anim = this->animation;
	this->animation = animation;
}

void Animator::rm_animation(Animation * anim) {
	anim->cancel_animation(this->cube);

	// If first animation
	if (this->animation == anim) {
		this->animation = anim->next_anim;
		return;
	}

	// If in the list
	Animation * current = this->animation;
	while (current != nullptr and current->next_anim != anim)
		current = current->next_anim;

	// Remove the animation from the list
	if (current != nullptr)
		current->next_anim = current->next_anim->next_anim;
}

void Animator::clear() {
	Animation * current = this->animation;
	Animation * next = nullptr;

	while (current != nullptr) {
		next = current->next_anim;
		delete current;
		current = next;
	}
}

void Animator::next_cycle() {
	Animation * current = this->animation;

	this->cycle += 1;
	while (current != nullptr) {
		bool to_continue = true;
		if (this->cycle % current->nb_cycles == 0) {
			// Execute the next frame
			to_continue = current->next_frame(this->cube);
		}

		if (to_continue) {
			// Normal cycle
			current = current->next_anim;
		} else {
			// On animation termination
			Animation * to_rm = current;
			current = current->next_anim;
			this->rm_animation(to_rm);
			delete to_rm;
		}
	}
}


// uint8_t dijkstra_mask[10];
// uint8_t tile_order[92];
#define COORD_MASK 0b01111111
#define STEP_MARKER 0b10000000

PropagationAnim::PropagationAnim(long frame_delay, uint8_t center, uint8_t r, uint8_t g, uint8_t b) : Animation(frame_delay) {
	this->colors[0] = r;this->colors[1] = g;this->colors[2] = b;
	// Init dijkstra
	uint8_t idx = center & COORD_MASK;
	for (uint8_t i=0 ; i<12 ; i++)
		this->dijkstra_mask[i] = 0;

	// Get the mask position
	uint8_t byte_idx = (center >> 3) & 0b1111;
	uint8_t bit_idx = center & 0b111;
	this->dijkstra_mask[byte_idx] |= 1 << bit_idx;
}

bool PropagationAnim::next_frame(Cube * cube) {
	// Unset the colors
	for (uint8_t idx=0 ; idx<96 ; idx++) {
		uint8_t byte_idx = (idx >> 3) & 0b1111;
		uint8_t bit_idx = idx & 0b111;
		uint8_t bit_and = (this->dijkstra_mask[byte_idx] >> bit_idx) & 0b1;

		// Serial.print(idx);Serial.print(" ");Serial.println(bit_and);
		if (bit_and != 0) {
			Coordinates coords(idx);
			cube->faces[coords.face()].set_pixel(
					coords.row(), coords.col(),
					0, 0, 0
			);			
		}
	}
	// Serial.println();

	// Set the new colors
	for (uint8_t idx=0 ; idx<96 ; idx++) {
		uint8_t byte_idx = (idx >> 3) & 0b1111;
		uint8_t bit_idx = idx & 0b111;
		uint8_t bit_and = (this->dijkstra_mask[byte_idx] >> bit_idx) & 0b1;
		// Serial.print(idx);Serial.print(" ");Serial.println(bit_and);

		if (bit_and != 0) {
			for (uint8_t dir=0 ; dir<4 ; dir++) {
				// Looks for neighbors
				Coordinates coords(idx);
				Coordinates::next_coord(coords, dir);

				uint8_t next_byte_idx = (coords.compact_coords >> 3) & 0b1111;
				uint8_t next_bit_idx = coords.compact_coords & 0b111;
				uint8_t next_bit_and = (this->dijkstra_mask[next_byte_idx] >> next_bit_idx) & 0b1;
				// Serial.print("\t");Serial.print(coords.compact_coords);Serial.print(" ");Serial.println(next_bit_and);

				// If unset neighbors
				if (next_bit_and == 0) {
					this->dijkstra_mask[next_byte_idx] |= 1 << next_bit_idx;
					cube->faces[coords.face()].set_pixel(
							coords.row(), coords.col(),
							this->colors[0], this->colors[1], this->colors[2]
					);
				}
			}
		}
	}
	// Serial.println();

	// Detect the end of the animation
	for (uint8_t i=0 ; i<12 ; i++)
		if (this->dijkstra_mask[i] != 0xFF)
			return true;

	// The end
	return false;
}








