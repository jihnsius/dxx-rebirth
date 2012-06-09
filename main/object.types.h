#pragma once

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

// WARNING!! If you add a type here, add its name to Object_type_names
// in object.c
#define MAX_OBJECT_TYPES    16

// Control types - what tells this object what do do
#define CT_NONE         0   // doesn't move (or change movement)
#define CT_AI           1   // driven by AI
#define CT_EXPLOSION    2   // explosion sequencer
#define CT_FLYING       4   // the player is flying
#define CT_SLEW         5   // slewing
#define CT_FLYTHROUGH   6   // the flythrough system
#define CT_WEAPON       9   // laser, etc.
#define CT_REPAIRCEN    10  // under the control of the repair center
#define CT_MORPH        11  // this object is being morphed
#define CT_DEBRIS       12  // this is a piece of debris
#define CT_POWERUP      13  // animating powerup blob
#define CT_LIGHT        14  // doesn't actually do anything
#define CT_REMOTE       15  // controlled by another net player
#define CT_CNTRLCEN     16  // the control center/main reactor

// Movement types
#define MT_NONE         0   // doesn't move
#define MT_PHYSICS      1   // moves by physics
#define MT_SPINNING     3   // this object doesn't move, just sits and spins

// Render types
#define RT_NONE         0   // does not render
#define RT_POLYOBJ      1   // a polygon model
#define RT_FIREBALL     2   // a fireball
#define RT_LASER        3   // a laser
#define RT_HOSTAGE      4   // a hostage
#define RT_POWERUP      5   // a powerup
#define RT_MORPH        6   // a robot being morphed
#define RT_WEAPON_VCLIP 7   // a weapon that renders as a vclip
