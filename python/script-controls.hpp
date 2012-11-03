#pragma once
#include "vecmat.h"
#include <stdint.h>
#include <boost/noncopyable.hpp>

struct script_control_info : boost::noncopyable {
	struct location : boost::noncopyable
	{
		vms_vector pos;
		uint16_t segment;
		bool enable_position, enable_segment;
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
	location glow_destination;
};
