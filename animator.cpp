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

