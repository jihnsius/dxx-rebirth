#include "player.hpp"
#include <boost/python/class.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/str.hpp>
#include <boost/format.hpp>
#include "defmodules.hpp"

#include "lighting.h"

DEFINE_DXX_OBJECT_SUBTYPE_STATIC(player_object);

using namespace boost::python;

struct container_lookup_player : public container_lookup_tmpl<player, MAX_PLAYERS + 4, Players> {};

struct player_container : public wrap_container_tmpl<player, container_lookup_player, player_container, false>
{
	typedef wrap_container_tmpl<player, container_lookup_player, player_container, false> base_container;
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
		return N_players;
	}
};

template <>
const char player_container::base_container::s_index_range_error[] = "index out of range for player";

template <>
void define_enum_values<player_object_type_t>(enum_<player_object_type_t>&)
{
}

template <typename T>
static const T& get_range_checked_object(const objnum_t objnum, const char *const errmsg)
{
	if (objnum >= Objects.size() || objnum > Highest_object_index)
	{
		PyErr_SetString(PyExc_IndexError, errmsg);
		throw_error_already_set();
	}
	return static_cast<const T&>(Objects[objnum]);
}

template <typename T, object_type_t type>
static const T& get_range_checked_typed_object(const objnum_t objnum, const char *const idxerrmsg, const char *const typeerrmsg)
{
	const T& r = get_range_checked_object<T>(objnum, idxerrmsg);
	if (r.type != type)
	{
		PyErr_SetString(PyExc_TypeError, typeerrmsg);
		throw_error_already_set();
	}
	return r;
}

template <fix player::*M, unsigned scale>
static fix scale_value(const player& player)
{
	return player.*M >> scale;
}

const dxxplayer_object& get_player_object(const player& player)
{
	static const char idxerrmsg[] = "index out of range for player object";
	static const char typeerrmsg[] = "unexpected type for player object";
	const objnum_t objnum = player.objnum;
	return get_range_checked_typed_object<dxxplayer_object, OBJ_PLAYER>(objnum, idxerrmsg, typeerrmsg);
}

static const vms_vector& get_player_object_position(const player& player)
{
	const dxxplayer_object& r = get_player_object(player);
	return r.pos;
}

template <unsigned flag>
static bool get_player_primary_weapon_flag(const player& player)
{
	return player.primary_weapon_flags & HAS_FLAG(flag);
}

template <unsigned flag>
static bool get_player_misc_flag(const player& player)
{
	return player.flags & flag;
}

static void set_player_headlight(const player& player, const unsigned value)
{
	if (&player != &Players[Player_num])
	{
		PyErr_SetString(PyExc_IndexError, "non-local player cannot be modified");
		throw_error_already_set();
	}
	if (!(player.flags & PLAYER_FLAGS_HEADLIGHT))
		return;
	if (!!(player.flags & PLAYER_FLAGS_HEADLIGHT_ON) == !!value)
		return;
	toggle_headlight_active();
}

template <unsigned N, char (player::*M)[N]>
static str get_string(const player& p)
{
	return p.*M;
}

void define_player_module(object& __main__, scope& scope_dxx)
{
	(void)__main__;
	class_<player, boost::noncopyable>("player", no_init)
		.add_property("callsign", &get_string<sizeof(player::callsign), &player::callsign>)
		.add_property("object", make_function(&get_player_object, return_internal_reference<>()))
		.add_property("raw_energy", &player::energy)
		.add_property("raw_shields", &player::shields)
		.add_property("energy", &scale_value<&player::energy, 16>)
		.add_property("shields", &scale_value<&player::shields, 16>)
		.add_property("laser_level", &player::laser_level)
		.add_property("score", &player::score)
		.add_property("hostages_on_board", &player::hostages_on_board)
		.add_property("pos", make_function(&get_player_object_position, return_internal_reference<>()))
		.add_property("has_weapon_vulcan", &get_player_primary_weapon_flag<VULCAN_INDEX>)
		.add_property("has_weapon_spreadfire", &get_player_primary_weapon_flag<SPREADFIRE_INDEX>)
		.add_property("has_weapon_plasma", &get_player_primary_weapon_flag<PLASMA_INDEX>)
		.add_property("has_weapon_fusion", &get_player_primary_weapon_flag<FUSION_INDEX>)
		.add_property("has_weapon_gauss", &get_player_primary_weapon_flag<GAUSS_INDEX>)
		.add_property("has_weapon_helix", &get_player_primary_weapon_flag<HELIX_INDEX>)
		.add_property("has_weapon_phoenix", &get_player_primary_weapon_flag<PHOENIX_INDEX>)
		.add_property("has_weapon_omega", &get_player_primary_weapon_flag<OMEGA_INDEX>)
		.add_property("has_key_blue", &get_player_misc_flag<PLAYER_FLAGS_BLUE_KEY>)
		.add_property("has_key_red", &get_player_misc_flag<PLAYER_FLAGS_RED_KEY>)
		.add_property("has_key_gold", &get_player_misc_flag<PLAYER_FLAGS_GOLD_KEY>)
		.add_property("has_full_map", &get_player_misc_flag<PLAYER_FLAGS_MAP_ALL>)
		.add_property("has_ammo_rack", &get_player_misc_flag<PLAYER_FLAGS_AMMO_RACK>)
		.add_property("has_converter", &get_player_misc_flag<PLAYER_FLAGS_CONVERTER>)
		.add_property("has_weapon_quad_laser", &get_player_misc_flag<PLAYER_FLAGS_QUAD_LASERS>)
		.add_property("cloaked", &get_player_misc_flag<PLAYER_FLAGS_CLOAKED>)
		.add_property("invulnerable", &get_player_misc_flag<PLAYER_FLAGS_INVULNERABLE>)
		.add_property("has_afterburner", &get_player_misc_flag<PLAYER_FLAGS_AFTERBURNER>)
		.add_property("has_headlight", &get_player_misc_flag<PLAYER_FLAGS_HEADLIGHT>)
		.add_property("headlight_on", &get_player_misc_flag<PLAYER_FLAGS_HEADLIGHT_ON>, &set_player_headlight)
		;
	DEFINE_COMMON_CONTAINER_EXPORTS(player_object);
	define_common_container_exports<player_container>(scope_dxx, "player_container", "players");
}

static player& get_local_player()
{
	return Players[Player_num];
}

static player& get_named_player(player_container&, str name)
{
	const std::string ns = extract<std::string>(name)();
	const char *const n = ns.c_str();
	for (unsigned u = N_players; u -- > 0;)
	{
		player& p = Players[u];
		if (!strcasecmp(p.callsign, n))
			return p;
	}
	PyErr_SetString(PyExc_KeyError, (boost::format("No such callsign: \"%s\"") % n).str().c_str());
	throw_error_already_set();
	// Unreached
	return Players[0];
}

template <class X1, class X2, class X3>
void define_specific_container_exports(scope&, const char *, class_<player_container, X1, X2, X3>& c)
{
	c.add_static_property("self", make_function(&get_local_player, return_value_policy<reference_existing_object>()))
		.def("__getitem__", make_function(&get_named_player, return_value_policy<reference_existing_object>()))
		;
}
