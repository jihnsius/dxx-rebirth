#include <boost/python/enum.hpp>
#include <boost/python/str.hpp>
#include <boost/format.hpp>
#include "wrap-container.hpp"
#include "defmodules.hpp"
#include "segment.h"
#include "gameseg.h"

using namespace boost::python;

struct container_lookup_segment : public strict_container_lookup_tmpl<segment, MAX_SEGMENTS, segment_array_t, Segments> {};

struct segment_container : public wrap_container_tmpl<segment, container_lookup_segment, segment_container, false>
{
	typedef wrap_container_tmpl<segment, container_lookup_segment, segment_container, false> base_container;
	const_iterator begin() const
	{
		return base_container::begin();
	}
	const_iterator end() const
	{
		return begin() + size();
	}
	size_type size() const
	{
		return Highest_segment_index + 1;
	}
};
template <>
const char segment_container::base_container::s_index_range_error[] = "index out of range for segment";

static uintptr_t get_raw_segment_address(const segment& s)
{
	return reinterpret_cast<uintptr_t>(&s);
}

static unsigned get_raw_segment_index(const segment& s)
{
	return &s - Segments;
}

static segnum_t get_segment_index(const segment& s)
{
	return &s - Segments;
}

template <typename R, R segment::*M>
static R get_contained_value(const segment& t)
{
	return t.*M;
}

template <typename R, R segment2::*M>
static R get_contained_value(const segment& t)
{
	return (*s2s2(&t)).*M;
}

static void define_enum_values(enum_<segment_type_t>& e)
{
	e.value("nothing", SEGMENT_IS_NOTHING)
		.value("energy", SEGMENT_IS_FUELCEN)
		.value("repair", SEGMENT_IS_REPAIRCEN)
		.value("control", SEGMENT_IS_CONTROLCEN)
		.value("matcen", SEGMENT_IS_ROBOTMAKER)
		.value("goal_blue", SEGMENT_IS_GOAL_BLUE)
		.value("goal_red", SEGMENT_IS_GOAL_RED);
}

static str pretty(const segnum_t& segnum) {
	return (boost::format("{%u} <dxx.segnum at %p>") % segnum.contained_value % &segnum).str().c_str();
}

void define_segment_module(object&, scope& s)
{
	enum_<segment_type_t> e("segment_type");
	define_enum_values(e);
	class_<segment, boost::noncopyable> c("segment", no_init);
	c.add_property("type", &get_contained_value<segment_type_t, &segment2::special>)
		.add_property("raw_address", &get_raw_segment_address)
		.add_property("raw_index", &get_raw_segment_index)
		.add_property("idx", &get_segment_index)
		;
	define_common_container_exports<segment_container>(s, "segment_container", "segments");
	class_<segnum_t>("segnum", init<const unsigned short&>())
		.def("__str__", &pretty);
		;
}
