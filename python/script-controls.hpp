#pragma once
#include "vecmat.h"

struct script_control_info {
	/*
	 * Turn the ship to focus on this spot.
	 */
	vms_vector ship_orientation_position;
	/*
	 * Turn the guided missile to focus on this spot.
	 */
	vms_vector guided_orientation_position;
	/*
	 * Fly the ship to reach this spot.
	 */
	vms_vector destination_position;
	bool enable_ship_orientation;
	bool enable_guided_orientation;
	bool enable_destination;
};
