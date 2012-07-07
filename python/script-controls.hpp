#pragma once
#include "vecmat.h"
#include <stdint.h>

struct script_control_info {
	struct location
	{
		vms_vector pos;
		uint16_t segment;
		bool enable;
	};
	/*
	 * Turn the ship to focus on this spot.
	 */
	location ship_orientation;
	/*
	 * Fly the guided missile to this spot.
	 */
	location guided_destination;
	/*
	 * Fly the ship to this spot.
	 */
	location ship_destination;
};
