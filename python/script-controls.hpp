#pragma once
#include "vecmat.h"
#include "segment.h"
#include <stdint.h>
#include <boost/noncopyable.hpp>
#include <array>

struct script_control_info : boost::noncopyable {
	struct location : boost::noncopyable
	{
		vms_vector pos;
		segnum_t segment;
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
	location glow_main;
	struct per_inset_window
	{
		location glow;
	};
	enum
	{
		count_inset_windows = 5 /* MAX_RENDERED_WINDOWS - 1 */
	};
	typedef std::array<per_inset_window, count_inset_windows> per_inset_window_container;
	per_inset_window_container inset;
};
