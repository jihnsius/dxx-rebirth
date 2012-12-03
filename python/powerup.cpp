#include "powerup.h"
#include "weapon.h"	// spit_powerup
#include "multi.h"	// multi_send_drop_weapon
#include "wrap-filter-container.hpp"
#include "wrap-object.hpp"
#include "filter.hpp"
#include <boost/python/def.hpp>
#include "defmodules.hpp"

using namespace boost::python;

DEFINE_DXX_OBJECT_TYPESAFE_SUBTYPE(powerup, OBJ_POWERUP);

static void script_drop_powerup(powerup_type_t type)
{
	const unsigned seed = d_rand();
	const objnum_t objnum = spit_powerup(ConsoleObject, type, seed);
	if ((Game_mode & GM_MULTI) && objnum != object_none)
		multi_send_drop_weapon(objnum, seed);
}

template <>
class_<dxxpowerup, bases<dxxobject>, boost::noncopyable>& define_class_properties<dxxpowerup, bases<dxxobject>, boost::noncopyable>(class_<dxxpowerup, bases<dxxobject>, boost::noncopyable>& c)
{
	scope s(c);
	def("drop", script_drop_powerup);
	return c;
}

template <>
void define_enum_values<powerup_type_t>(enum_<powerup_type_t>& e)
{
	e.value("extra_life", POW_EXTRA_LIFE)
		.value("energy", POW_ENERGY)
		.value("shield", POW_SHIELD_BOOST)
		.value("laser", POW_LASER)
		.value("key_blue", POW_KEY_BLUE)
		.value("key_red", POW_KEY_RED)
		.value("key_gold", POW_KEY_GOLD)
		.value("missile_1", POW_MISSILE_1)
		.value("missile_4", POW_MISSILE_4)
		.value("quad_laser", POW_QUAD_FIRE)
		.value("vulcan", POW_VULCAN_WEAPON)
		.value("spreadfire", POW_SPREADFIRE_WEAPON)
		.value("plasma", POW_PLASMA_WEAPON)
		.value("fusion", POW_FUSION_WEAPON)
		.value("proximity_bomb", POW_PROXIMITY_WEAPON)
		.value("smart_missile", POW_SMARTBOMB_WEAPON)
		.value("mega_missile", POW_MEGA_WEAPON)
		.value("vulcan_ammo", POW_VULCAN_AMMO)
		.value("homing_1", POW_HOMING_AMMO_1)
		.value("homing_4", POW_HOMING_AMMO_4)
		.value("cloak", POW_CLOAK)
		.value("nae_turbo", POW_TURBO)		// No apparent effect
		.value("invulnerability", POW_INVULNERABILITY)
		//	Invisible super item that grants lots of everything.
		.value("nae_megawow", POW_MEGAWOW)
		.value("gauss", POW_GAUSS_WEAPON)
		.value("helix", POW_HELIX_WEAPON)
		.value("phoenix", POW_PHOENIX_WEAPON)
		.value("omega", POW_OMEGA_WEAPON)
		.value("super_laser", POW_SUPER_LASER)
		.value("full_map", POW_FULL_MAP)
		.value("converter", POW_CONVERTER)
		.value("ammo_rack", POW_AMMO_RACK)
		.value("afterburner", POW_AFTERBURNER)
		.value("headlight", POW_HEADLIGHT)
		.value("flash_1", POW_SMISSILE1_1)
		.value("flash_4", POW_SMISSILE1_4)
		.value("guided_1", POW_GUIDED_MISSILE_1)
		.value("guided_4", POW_GUIDED_MISSILE_4)
		.value("smart_mine", POW_SMART_MINE)
		.value("mercury_1", POW_MERCURY_MISSILE_1)
		.value("mercury_4", POW_MERCURY_MISSILE_4)
		.value("earthshaker", POW_EARTHSHAKER_MISSILE)
		.value("flag_blue", POW_FLAG_BLUE)
		.value("flag_red", POW_FLAG_RED)
		.value("hoard_orb", POW_HOARD_ORB);
}

void define_powerup_module(object& __main__, scope& scope_dxx)
{
	DEFINE_COMMON_CONTAINER_EXPORTS(powerup);
}
