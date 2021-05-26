#include "animator.hpp"



Animator::Animator(Cube * cube) {
	this->cube = cube;
	this->cycle = 0;
	this->animation = nullptr;
}

void Animator::add_animation(Animation * animation) {
	if (this->animation != nullptr) {
		this->animation->cancel_animation(this->cube);
		delete this->animation;
	}

	this->animation = animation;
}

void Animator::rm_animation(bool delete_anim) {
	if (delete_anim and animation != nullptr) {
		this->animation->cancel_animation(this->cube);
		delete this->animation;
	}
	this->animation = nullptr;
}

void Animator::next_cycle() {
	if (this->animation == nullptr)
		return;
	
	this->cycle += 1;

	if (this->animation->nb_cycles == this->cycle) {
		this->cycle = 0;
		this->animation->next_frame(this->cube);
	}
}

