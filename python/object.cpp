#include "filter.hpp"
#include "wrap-object.hpp"
#include "defmodules.hpp"

#include "fvi.h"

using namespace boost::python;

struct object_container : public wrap_container_tmpl<dxxobject, container_lookup_object, object_container, false>
{
	typedef wrap_container_tmpl<dxxobject, container_lookup_object, object_container, false> base_container;
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
		return Highest_object_index + 1;
	}
};
template <>
const char object_container::base_container::s_index_range_error[] = "index out of range for object";

static uintptr_t get_raw_dxxobject_address(const dxxobject& o)
{
	return reinterpret_cast<uintptr_t>(&o);
}

static unsigned get_raw_dxxobject_index(const dxxobject& o)
{
	return &o - Objects;
}

template <typename T, typename R, typename CT, CT T::*M>
static R get_contained_value(const T& t)
{
	return static_cast<R>(t.*M);
}

template <>
void define_enum_values<object_type_t>(enum_<object_type_t>& e)
{
	e.value("none", OBJ_NONE)
		.value("wall", OBJ_WALL)
		.value("fireball", OBJ_FIREBALL)
		.value("robot", OBJ_ROBOT)
		.value("hostage", OBJ_HOSTAGE)
		.value("player", OBJ_PLAYER)
		.value("weapon", OBJ_WEAPON)
		.value("camera", OBJ_CAMERA)
		.value("powerup", OBJ_POWERUP)
		.value("debris", OBJ_DEBRIS)
		.value("control_center", OBJ_CNTRLCEN)
		.value("flare", OBJ_FLARE)
		.value("clutter", OBJ_CLUTTER)
		.value("ghost", OBJ_GHOST)
		.value("light", OBJ_LIGHT)
		.value("coop", OBJ_COOP)
		.value("marker", OBJ_MARKER);
}

static dxxobject *check_exists_passable_direct_path(const dxxobject& src, const dxxobject& dst)
{
	fvi_query fq;
	fvi_info hit_data;
	fq.p0 					= &src.pos;
	fq.p1 					= &dst.pos;
	fq.rad 					= 0;
	fq.thisobjnum			= &src - Objects;
	fq.flags 				= FQ_CHECK_OBJS | FQ_GET_SEGLIST;
	fq.startseg				= src.segnum;
	fq.ignore_obj_list	= NULL;
	if (find_vector_intersection(&fq, &hit_data) != HIT_OBJECT)
		return NULL;
	return &Objects[hit_data.hit_object];
}

void define_object_base_class(scope& s)
{
	enum_<object_type_t> e("object_type");
	define_enum_values(e);
	class_<dxxobject, boost::noncopyable> c("object", no_init);
	c.add_property("objtype", &get_contained_value<dxxobject, object_type_t, ubyte, &dxxobject::type>)
		.add_property("raw_address", &get_raw_dxxobject_address)
		.add_property("raw_index", &get_raw_dxxobject_index)
		.add_property("signature", &dxxobject::signature)
		.add_property("control_type", &dxxobject::control_type)
		.add_property("movement_type", &dxxobject::movement_type)
		.add_property("render_type", &dxxobject::render_type)
		.add_property("segnum", &dxxobject::segnum)
		.add_property("pos", &dxxobject::pos)
		.add_property("orient", &dxxobject::orient)
		.def_readwrite("size", &dxxobject::size)
		.add_property("shields", &dxxobject::shields)
		.add_property("last_pos", &dxxobject::last_pos)
		.add_property("contains_type", &get_contained_value<dxxobject, object_type_t, sbyte, &dxxobject::contains_type>)
		.add_property("contains_id", &dxxobject::contains_id)
		.add_property("contains_count", &dxxobject::contains_count)
		.add_property("lifeleft", &dxxobject::lifeleft)
		.def("passable_direct_path", &check_exists_passable_direct_path, return_internal_reference<>())
		;
	define_common_container_exports<object_container>(s, "object_container", "objects");
}
