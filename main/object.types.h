#pragma once
#include <boost/operators.hpp>
#include <boost/serialization/strong_typedef.hpp>

enum object_type_t
{
// Object types
OBJ_NONE        = 255, // unused object
OBJ_WALL        = 0,   // A wall... not really an object, but used for collisions
OBJ_FIREBALL    = 1,   // a fireball, part of an explosion
OBJ_ROBOT       = 2,   // an evil enemy
OBJ_HOSTAGE     = 3,   // a hostage you need to rescue
OBJ_PLAYER      = 4,   // the player on the console
OBJ_WEAPON      = 5,   // a laser, missile, etc
OBJ_CAMERA      = 6,   // a camera to slew around with
OBJ_POWERUP     = 7,   // a powerup you can pick up
OBJ_DEBRIS      = 8,   // a piece of robot
OBJ_CNTRLCEN    = 9,   // the control center
OBJ_FLARE       = 10,  // a flare
OBJ_CLUTTER     = 11,  // misc objects
OBJ_GHOST       = 12,  // what the player turns into when dead
OBJ_LIGHT       = 13,  // a light source, & not much else
OBJ_COOP        = 14,  // a cooperative player object.
OBJ_MARKER      = 15,  // a map marker
};

typedef enum object_type_t object_type_t;

// WARNING!! If you add a type here, add its name to Object_type_names
// in object.c
#define MAX_OBJECT_TYPES    16

enum control_type_t
{
// Control types - what tells this object what do do
CT_NONE       = 0,   // doesn't move (or change movement)
CT_AI         = 1,   // driven by AI
CT_EXPLOSION  = 2,   // explosion sequencer
CT_FLYING     = 4,   // the player is flying
CT_SLEW       = 5,   // slewing
CT_FLYTHROUGH = 6,   // the flythrough system
CT_WEAPON     = 9,   // laser, etc.
CT_REPAIRCEN  = 10,  // under the control of the repair center
CT_MORPH      = 11,  // this object is being morphed
CT_DEBRIS     = 12,  // this is a piece of debris
CT_POWERUP    = 13,  // animating powerup blob
CT_LIGHT      = 14,  // doesn't actually do anything
CT_REMOTE     = 15,  // controlled by another net player
CT_CNTRLCEN   = 16,  // the control center/main reactor
};

enum movement_type_t
{
// Movement types
MT_NONE       = 0,  // doesn't move
MT_PHYSICS    = 1,  // moves by physics
MT_SPINNING   = 3,  // this object doesn't move, just sits and spins
};

enum render_type_t
{
// Render types
RT_NONE        =0,  // does not render
RT_POLYOBJ     =1,  // a polygon model
RT_FIREBALL    =2,  // a fireball
RT_LASER       =3,  // a laser
RT_HOSTAGE     =4,  // a hostage
RT_POWERUP     =5,  // a powerup
RT_MORPH       =6,  // a robot being morphed
RT_WEAPON_VCLIP=7,  // a weapon that renders as a vclip
};

#define DECLARE_OBJECT_INDEX(N,V)	DECLARE_TYPESAFE_INDEX(object,N,V)

DECLARE_OBJECT_INDEX(first, 0);
DECLARE_OBJECT_INDEX(none, 0xffff);
DECLARE_OBJECT_INDEX(guidebot_cannot_reach, 0xfffe);

struct Highest_object_index_t
{
	unsigned contained_value;
	void operator=(const unsigned & rhs) { contained_value = rhs; }
	operator const unsigned & () const { return contained_value; }
	Highest_object_index_t& operator--() { -- contained_value; return *this; }
};

struct Num_objects_t
{
	unsigned contained_value;
	void operator=(const unsigned & rhs) { contained_value = rhs; }
	operator const unsigned & () const { return contained_value; }
	Highest_object_index_t operator--() { -- contained_value; Highest_object_index_t o; o = contained_value; return o; }
	Highest_object_index_t operator++(int) { Highest_object_index_t o; o = contained_value; contained_value ++; return o; }
};

BOOST_STRONG_TYPEDEF(unsigned, object_step_interval_t);

#ifdef DXX_USE_STRICT_TYPESAFE
/*
 * This is based on BOOST_STRONG_TYPEDEF, but that macro does not permit
 * sufficient customization for the required use cases.
 */
struct objnum_t
	: boost::totally_ordered1< objnum_t
	>
{
	enum
	{
		object_first = typesafe_idx_object::first,
		object_none = typesafe_idx_object::none,
		object_guidebot_cannot_reach = typesafe_idx_object::guidebot_cannot_reach,
	};
	unsigned short contained_value;
	explicit objnum_t(const unsigned short t_) : contained_value(t_) {};
	objnum_t() = default;
	objnum_t(const Highest_object_index_t& i) : contained_value(i.contained_value) {}
	objnum_t & operator=(const Highest_object_index_t & rhs) { contained_value = rhs.contained_value; return *this;}
	operator unsigned () const { return contained_value; }
	bool operator==(const objnum_t & rhs) const { return contained_value == rhs.contained_value; }
	bool operator<(const objnum_t & rhs) const { return (contained_value < rhs.contained_value); }
	objnum_t& operator++() { ++ contained_value; return *this; }
	objnum_t& operator--() { -- contained_value; return *this; }
	objnum_t operator++(int) { objnum_t o = *this; contained_value ++; return o; }
	objnum_t operator--(int) { objnum_t o = *this; contained_value --; return o; }
	objnum_t& operator+=(const object_step_interval_t& i) { contained_value += i; return *this; }
	DEFINE_CONSTRUCT_SPECIAL(objnum_t, object_first);
	DEFINE_CONSTRUCT_SPECIAL(objnum_t, object_none);
	DEFINE_CONSTRUCT_SPECIAL(objnum_t, object_guidebot_cannot_reach);
	DEFINE_COMPARE_SPECIAL(==, object_none);
	DEFINE_COMPARE_SPECIAL(!=, object_none);
	DEFINE_COMPARE_SPECIAL(==, object_first);
	DEFINE_COMPARE_SPECIAL(!=, object_first);
	DEFINE_COMPARE_SPECIAL(==, object_guidebot_cannot_reach);
	DEFINE_COMPARE_PASSTHROUGH(<=, Highest_object_index_t);
	DEFINE_COMPARE_PASSTHROUGH(==, Highest_object_index_t);
	DEFINE_COMPARE_PASSTHROUGH(>, Highest_object_index_t);
	DEFINE_COMPARE_PASSTHROUGH(<, Num_objects_t);
	DEFINE_COMPARE_PASSTHROUGH(>=, Num_objects_t);
	template <typename T> objnum_t & operator=(T) = delete;
	template <typename T> bool operator==(T) const = delete;
	template <typename T> bool operator!=(T) const = delete;
	template <typename T> bool operator<=(T) const = delete;
	template <typename T> bool operator>=(T) const = delete;
	template <typename T> bool operator>(T) const = delete;
	template <typename T> bool operator<(T) const = delete;
};
static inline bool strict_less_highest_object(const objnum_t& o, const Highest_object_index_t& i) { return (o.contained_value < i.contained_value); }
#else
typedef unsigned short objnum_t;
static inline bool strict_less_highest_object(const objnum_t& o, const Highest_object_index_t& i) { return (o < i.contained_value); }
#endif
static inline objnum_t objnum_t_from_num_objects(const Num_objects_t& i) { return objnum_t(i.contained_value); }
