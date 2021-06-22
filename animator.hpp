#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "cube.hpp"
#include "Arduino.h"

class Animation {
public:
	// Nb cycles between 2 cycles
	uint8_t nb_cycles;
	Animation * next_anim;

	Animation(long frame_delay)
			: nb_cycles(frame_delay / REFRESH_DELAY)
			, next_anim(nullptr)
	{};
	virtual ~Animation(){};
	/** Stop the animation and reset the state before animation.
	  */
	virtual void cancel_animation(Cube * cube);
	/** Execute the next frame of the animation
	  * If the animation ends after this frame, it should return false
	  * @return true if the animation must continue
	  */
	virtual bool next_frame(Cube * cube);
};


class Animator {
private:
	Cube * cube;
	uint8_t cycle;

	// Only 1 animation at a time
	Animation * animation;

public:
	Animator(Cube * cube);
	/** Add an animation to the animation list
	  */
	void add_animation(Animation * animation);
	/** Remove (but not delete) the animation from the animation list.
	  * It calls the cancel_animation method of the animation
	  */
	void rm_animation(Animation * anim);
	/** Tic the clock and trigger all the animations that needs refresh.
	  * Each animation that terminates are automatically removed and deleted.
	  */
	void next_cycle();
};

#endif