#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "cube.hpp"


class Animation {
public:
	// Nb cycles between 2 cycles
	uint8_t nb_cycles;

	virtual Animation(long frame_delay) {
		this->nb_cycles = frame_delay / REFRESH_DELAY;
	}
	virtual ~Animation(){};
	virtual void cancel_animation(Cube * cube);

	virtual void next_frame(Cube * cube);
};


class Animator {
private:
	Cube * cube;
	uint8_t cycle;

	// Only 1 animation at a time
	Animation * animation;

public:
	Animator(Cube * cube);
	void add_animation(Animation * animation);
	void rm_animation(bool delete_anim);
	void next_cycle();
};

#endif