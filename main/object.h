/* $Id: object.h,v 1.1.1.1 2006/03/17 19:56:44 zicodxx Exp $ */
/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * object system definitions
 *
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#include "pstypes.h"
#include "vecmat.h"
#include "segment.h"
#include "gameseg.h"
#include "aistruct.h"
#include "gr.h"
#include "piggy.h"

/*
 * CONSTANTS
 */

#define MAX_OBJECTS     350 // increased on 01/24/95 for multiplayer. --MK;  total number of objects in world
#define MAX_USED_OBJECTS	(MAX_OBJECTS-20)

#include "object.types.h"

// Result types
#define RESULT_NOTHING  0   // Ignore this collision
#define RESULT_CHECK    1   // Check for this collision

// misc object flags
#define OF_EXPLODING        1   // this object is exploding
#define OF_SHOULD_BE_DEAD   2   // this object should be dead, so next time we can, we should delete this object.
#define OF_DESTROYED        4   // this has been killed, and is showing the dead version
#define OF_SILENT           8   // this makes no sound when it hits a wall.  Added by MK for weapons, if you extend it to other types, do it completely!
#define OF_ATTACHED         16  // this object is a fireball attached to another object
#define OF_HARMLESS         32  // this object does no damage.  Added to make quad lasers do 1.5 damage as normal lasers.
#define OF_PLAYER_DROPPED   64  // this object was dropped by the player...

// Different Weapon ID types...
#define WEAPON_ID_LASER         0
#define WEAPON_ID_MISSLE        1
#define WEAPON_ID_CANNONBALL    2

// Object Initial shields...
#define OBJECT_INITIAL_SHIELDS F1_0/2

// physics flags
#define PF_TURNROLL         0x01    // roll when turning
#define PF_LEVELLING        0x02    // level object with closest side
#define PF_BOUNCE           0x04    // bounce (not slide) when hit will
#define PF_WIGGLE           0x08    // wiggle while flying
#define PF_STICK            0x10    // object sticks (stops moving) when hits wall
#define PF_PERSISTENT       0x20    // object keeps going even after it hits another object (eg, fusion cannon)
#define PF_USES_THRUST      0x40    // this object uses its thrust
#define PF_BOUNCED_ONCE     0x80    // Weapon has bounced once.
#define PF_FREE_SPINNING    0x100   // Drag does not apply to rotation of this object
#define PF_BOUNCES_TWICE    0x200   // This weapon bounces twice, then dies

#define IMMORTAL_TIME   0x3fffffff  // Time assigned to immortal objects, about 32768 seconds, or about 9 hours.

extern const char Object_type_names[MAX_OBJECT_TYPES][9];

// List of objects rendered last frame in order.  Created at render
// time, used by homing missiles in laser.c
#define MAX_RENDERED_OBJECTS    50

/*
 * STRUCTURES
 */

// A compressed form for sending crucial data
typedef struct shortpos {
	sbyte   bytemat[9];
	short   xo,yo,zo;
	short   segment;
	short   velx, vely, velz;
} __pack__ shortpos;

// This is specific to the shortpos extraction routines in gameseg.c.
#define RELPOS_PRECISION    10
#define MATRIX_PRECISION    9
#define MATRIX_MAX          0x7f    // This is based on MATRIX_PRECISION, 9 => 0x7f

template <typename T>
struct object_array_template_t
{
	typedef std::array<T, MAX_OBJECTS> array_t;
	array_t a;
	T& operator[](const objnum_t& o) { Assert(static_cast<unsigned>(o) < a.size()); return a[o]; }
#ifdef DXX_USE_STRICT_TYPESAFE
	T& operator[](object_first_type_t) { return a[typesafe_idx_object::first]; }
	template <typename U> void operator[](U) = delete;
#endif
	T& operator[](const Highest_object_index_t& i) { return a[i.contained_value]; }
	void fill(const T& t) { a.fill(t); }
	objnum_t idx(const T *p) const
	{
		return objnum_t(std::distance(&*a.begin(), p));
	}
	Num_objects_t size() const {
		Num_objects_t n;
		n.contained_value = (unsigned)a.size();
		return n;
	}
};

template <typename T>
static inline objnum_t operator-(const T *p, const object_array_template_t<T>& a)
{
	return a.idx(p);
}

// information for physics sim for an object
typedef struct physics_info {
	vms_vector  velocity;   // velocity vector of this object
	vms_vector  thrust;     // constant force applied to this object
	fix         mass;       // the mass of this object
	fix         drag;       // how fast this slows down
	fix         brakes;     // how much brakes applied
	vms_vector  rotvel;     // rotational velecity (angles)
	vms_vector  rotthrust;  // rotational acceleration
	fixang      turnroll;   // rotation caused by turn banking
	union {
		ushort      flags;      // misc physics flags
		struct {
			ubyte flag_turnroll : 1,
				  flag_levelling : 1,
				  flag_bounce : 1,
				  flag_wiggle : 1,
				  flag_stick : 1,
				  flag_persistent : 1,
				  flag_uses_thrust : 1,
				  flag_bounced_once : 1,
				  flag_free_spinning : 1,
				  flag_bounces_twice : 1;
		};
	};
} __pack__ physics_info;

// stuctures for different kinds of simulation

typedef struct laser_info {
	short   parent_type;        // The type of the parent of this object
	objnum_t   parent_num;         // The object's parent's number
	int     parent_signature;   // The object's parent's signature...
	fix64   creation_time;      // Absolute time of creation.
	objnum_t   last_hitobj;        // For persistent weapons (survive object collision), object it most recently hit.
	object_array_template_t<ubyte>   hitobj_list; // list of all objects persistent weapon has already damaged (useful in case it's in contact with two objects at the same time)
	objnum_t   track_goal;         // Object this object is tracking.
	fix     multiplier;         // Power if this is a fusion bolt (or other super weapon to be added).
} laser_info;

// Same as above but structure Savegames/Multiplayer objects expect
typedef struct laser_info_rw {
	short   parent_type;        // The type of the parent of this object
	short   parent_num;         // The object's parent's number
	int     parent_signature;   // The object's parent's signature...
	fix     creation_time;      // Absolute time of creation.
	short   last_hitobj;        // For persistent weapons (survive object collision), object it most recently hit.
	short   track_goal;         // Object this object is tracking.
	fix     multiplier;         // Power if this is a fusion bolt (or other super weapon to be added).
} __pack__ laser_info_rw;

typedef struct explosion_info {
    fix     spawn_time;         // when lifeleft is < this, spawn another
    fix     delete_time;        // when to delete object
    objnum_t   delete_objnum;      // and what object to delete
    objnum_t   attach_parent;      // explosion is attached to this object
    objnum_t   prev_attach;        // previous explosion in attach list
    objnum_t   next_attach;        // next explosion in attach list
} explosion_info;

typedef struct explosion_info_rw {
    fix     spawn_time;         // when lifeleft is < this, spawn another
    fix     delete_time;        // when to delete object
    short   delete_objnum;      // and what object to delete
    short   attach_parent;      // explosion is attached to this object
    short   prev_attach;        // previous explosion in attach list
    short   next_attach;        // next explosion in attach list
} __pack__ explosion_info_rw;

typedef struct light_info {
    fix     intensity;          // how bright the light is
} __pack__ light_info;

#define PF_SPAT_BY_PLAYER   1   //this powerup was spat by the player

typedef struct powerup_info {
	int     count;          // how many/much we pick up (vulcan cannon only?)
	fix64   creation_time;  // Absolute time of creation.
	int     flags;          // spat by player?
} __pack__ powerup_info;

// Same as above but structure Savegames/Multiplayer objects expect
typedef struct powerup_info_rw {
	int     count;          // how many/much we pick up (vulcan cannon only?)
	fix     creation_time;  // Absolute time of creation.
	int     flags;          // spat by player?
} __pack__ powerup_info_rw;

typedef struct vclip_info {
	int     vclip_num;
	fix     frametime;
	sbyte   framenum;
} __pack__ vclip_info;

// structures for different kinds of rendering

typedef struct polyobj_info {
	int     model_num;          // which polygon model
	vms_angvec anim_angles[MAX_SUBMODELS]; // angles for each subobject
	int     subobj_flags;       // specify which subobjs to draw
	int     tmap_override;      // if this is not -1, map all face to this
	int     alt_textures;       // if not -1, use these textures instead
} __pack__ polyobj_info;

typedef struct dxxobject {
	int     signature;      // Every object ever has a unique signature...
	ubyte   type;           // what type of object this is... robot, weapon, hostage, powerup, fireball
	ubyte   id;             // which form of object...which powerup, robot, etc.
#ifdef WORDS_NEED_ALIGNMENT
	short   pad;
#endif
	objnum_t   next,prev;      // id of next and previous connected object in Objects, -1 = no connection
	ubyte   control_type;   // how this object is controlled
	ubyte   movement_type;  // how this object moves
	ubyte   render_type;    // how this object renders
	ubyte   flags;          // misc flags
	segnum_t   segnum;         // segment number containing object
	objnum_t   attached_obj;   // number of attached fireball object
	vms_vector pos;         // absolute x,y,z coordinate of center of object
	vms_matrix orient;      // orientation of object in world
	fix     size;           // 3d size of object - for collision detection
	fix     shields;        // Starts at maximum, when <0, object dies..
	vms_vector last_pos;    // where object was last frame
	sbyte   contains_type;  // Type of object this object contains (eg, spider contains powerup)
	sbyte   contains_id;    // ID of object this object contains (eg, id = blue type = key)
	sbyte   contains_count; // number of objects of type:id this object contains
	sbyte   matcen_creator; // Materialization center that created this object, high bit set if matcen-created
	fix     lifeleft;       // how long until goes away, or 7fff if immortal
	// -- Removed, MK, 10/16/95, using lifeleft instead: int     lightlevel;

	// movement info, determined by MOVEMENT_TYPE
	union {
		physics_info phys_info; // a physics object
		vms_vector   spin_rate; // for spinning objects
	} mtype;

	// control info, determined by CONTROL_TYPE
	union {
		struct laser_info      laser_info;
		struct explosion_info  expl_info;      // NOTE: debris uses this also
		struct ai_static       ai_info;
		struct light_info      light_info;     // why put this here?  Didn't know what else to do with it.
		struct powerup_info    powerup_info;
	} ctype ;

	// render info, determined by RENDER_TYPE
	union {
		struct polyobj_info    pobj_info;      // polygon model
		struct vclip_info      vclip_info;     // vclip
	} rtype ;

#ifdef WORDS_NEED_ALIGNMENT
	short   pad2;
#endif
} dxxobject;

// Same as above but structure Savegames/Multiplayer objects expect
typedef struct object_rw {
	int     signature;      // Every object ever has a unique signature...
	ubyte   type;           // what type of object this is... robot, weapon, hostage, powerup, fireball
	ubyte   id;             // which form of object...which powerup, robot, etc.
#ifdef WORDS_NEED_ALIGNMENT
	short   pad;
#endif
	short   next,prev;      // id of next and previous connected object in Objects, -1 = no connection
	ubyte   control_type;   // how this object is controlled
	ubyte   movement_type;  // how this object moves
	ubyte   render_type;    // how this object renders
	ubyte   flags;          // misc flags
	short   segnum;         // segment number containing object
	short   attached_obj;   // number of attached fireball object
	vms_vector pos;         // absolute x,y,z coordinate of center of object
	vms_matrix orient;      // orientation of object in world
	fix     size;           // 3d size of object - for collision detection
	fix     shields;        // Starts at maximum, when <0, object dies..
	vms_vector last_pos;    // where object was last frame
	sbyte   contains_type;  // Type of object this object contains (eg, spider contains powerup)
	sbyte   contains_id;    // ID of object this object contains (eg, id = blue type = key)
	sbyte   contains_count; // number of objects of type:id this object contains
	sbyte   matcen_creator; // Materialization center that created this object, high bit set if matcen-created
	fix     lifeleft;       // how long until goes away, or 7fff if immortal
	// -- Removed, MK, 10/16/95, using lifeleft instead: int     lightlevel;

	// movement info, determined by MOVEMENT_TYPE
	union {
		physics_info phys_info; // a physics object
		vms_vector   spin_rate; // for spinning objects
	} __pack__ mtype ;

	// control info, determined by CONTROL_TYPE
	union {
		laser_info_rw   laser_info;
		explosion_info_rw  expl_info;      // NOTE: debris uses this also
		ai_static_rw    ai_info;
		struct light_info      light_info;     // why put this here?  Didn't know what else to do with it.
		powerup_info_rw powerup_info;
	} __pack__ ctype ;

	// render info, determined by RENDER_TYPE
	union {
		polyobj_info    pobj_info;      // polygon model
		struct vclip_info      vclip_info;     // vclip
	} __pack__ rtype;

#ifdef WORDS_NEED_ALIGNMENT
	short   pad2;
#endif
} __pack__ object_rw;
static_assert(sizeof(object_rw) == 264, "sizeof(object_rw)");

typedef struct obj_position {
	vms_vector  pos;        // absolute x,y,z coordinate of center of object
	vms_matrix  orient;     // orientation of object in world
	segnum_t       segnum;     // segment number containing object
} obj_position;

typedef struct {
	int     frame;
	dxxobject  *viewer;
	int     rear_view;
	int     user;
	int     num_objects;
	objnum_t   rendered_objects[MAX_RENDERED_OBJECTS];
} window_rendered_data;

#define MAX_RENDERED_WINDOWS    6

extern window_rendered_data Window_rendered_data[MAX_RENDERED_WINDOWS];

/*
 * VARIABLES
 */

extern int Object_next_signature;   // The next signature for the next newly created object

extern ubyte CollisionResult[MAX_OBJECT_TYPES][MAX_OBJECT_TYPES];
// ie CollisionResult[a][b]==  what happens to a when it collides with b

typedef object_array_template_t<dxxobject> object_array_t;

extern object_array_t Objects;
extern Highest_object_index_t Highest_object_index;    // highest objnum
extern Num_objects_t num_objects;

extern char *robot_names[];         // name of each robot

extern int Num_robot_types;

extern dxxobject *ConsoleObject;       // pointer to the object that is the player
extern dxxobject *Viewer;              // which object we are seeing from
extern dxxobject *Dead_player_camera;

extern int Player_is_dead;          // !0 means player is dead!
extern int Player_exploded;
extern int Player_eggs_dropped;
extern int Death_sequence_aborted;
extern objnum_t Player_fired_laser_this_frame;

/*
 * FUNCTIONS
 */

#ifdef __cplusplus

// do whatever setup needs to be done
void init_objects();

// returns segment number object is in.  Searches out from object's current
// seg, so this shouldn't be called if the object has "jumped" to a new seg
int obj_get_new_seg(dxxobject *obj);

// when an object has moved into a new segment, this function unlinks it
// from its old segment, and links it into the new segment
void obj_relink(objnum_t objnum,segnum_t newsegnum);

// for getting out of messed up linking situations (i.e. caused by demo playback)
void obj_relink_all(void);

// move an object from one segment to another. unlinks & relinks
void obj_set_new_seg(int objnum,segnum_t newsegnum);

// links an object into a segment's list of objects.
// takes object number and segment number
void obj_link(objnum_t objnum,segnum_t segnum);

// unlinks an object from a segment's list of objects
void obj_unlink(objnum_t objnum);

// initialize a new object.  adds to the list for the given segment
// returns the object number
objnum_t obj_create(enum object_type_t type, ubyte id, segnum_t segnum, const vms_vector *pos,
               const vms_matrix *orient, fix size,
               ubyte ctype, ubyte mtype, ubyte rtype);

// make a copy of an object. returs num of new object
objnum_t obj_create_copy(objnum_t objnum, vms_vector *new_pos, segnum_t newsegnum);

// remove object from the world
void obj_delete(objnum_t objnum);

// called after load.  Takes number of objects, and objects should be
// compressed
void reset_objects(int n_objs);

// make object array non-sparse
void compress_objects(void);

// Render an object.  Calls one of several routines based on type
void render_object(dxxobject *obj);

// Draw a blob-type object, like a fireball
void draw_object_blob(dxxobject *obj, bitmap_index bitmap);

// draw an object that is a texture-mapped rod
void draw_object_tmap_rod(dxxobject *obj, bitmap_index bitmap, int lighted);

// Deletes all objects that have been marked for death.
void obj_delete_all_that_should_be_dead();

// Toggles whether or not lock-boxes draw.
void object_toggle_lock_targets();

// move all objects for the current frame
void object_move_all();     // moves all objects

// set viewer object to next object in array
void object_goto_next_viewer();

// draw target boxes for nearby robots
void object_render_targets(void);

// move an object for the current frame
void object_move_one(dxxobject * obj);

// make object0 the player, setting all relevant fields
void init_player_object();

// check if object is in object->segnum.  if not, check the adjacent
// segs.  if not any of these, returns false, else sets obj->segnum &
// returns true callers should really use find_vector_intersection()
// Note: this function is in gameseg.c
extern int update_object_seg(dxxobject *obj);


// Finds what segment *obj is in, returns segment number.  If not in
// any segment, returns -1.  Note: This function is defined in
// gameseg.h, but object.h depends on gameseg.h, and object.h is where
// object is defined...get it?
extern segnum_t find_object_seg(dxxobject * obj );

// go through all objects and make sure they have the correct segment
// numbers used when debugging is on
void fix_object_segs();

// Drops objects contained in objp.
objnum_t object_create_egg(dxxobject *objp);

// Interface to object_create_egg, puts count objects of type type, id
// = id in objp and then drops them.
objnum_t call_object_create_egg(dxxobject *objp, int count, int type, int id);

extern void dead_player_end(void);

// Extract information from an object (objp->orient, objp->pos,
// objp->segnum), stuff in a shortpos structure.  See typedef
// shortpos.
extern void create_shortpos(shortpos *spp, dxxobject *objp, int swap_bytes);

// Extract information from a shortpos, stuff in objp->orient
// (matrix), objp->pos, objp->segnum
extern void extract_shortpos(dxxobject *objp, shortpos *spp, int swap_bytes);

// delete objects, such as weapons & explosions, that shouldn't stay
// between levels if clear_all is set, clear even proximity bombs
void clear_transient_objects(int clear_all);

// Returns a new, unique signature for a new object
int obj_get_signature();

// returns the number of a free object, updating Highest_object_index.
// Generally, obj_create() should be called to get an object, since it
// fills in important fields and does the linking.  returns -1 if no
// free objects
objnum_t obj_allocate(void);

// frees up an object.  Generally, obj_delete() should be called to
// get rid of an object.  This function deallocates the object entry
// after the object has been unlinked
void obj_free(objnum_t objnum);

// after calling init_object(), the network code has grabbed specific
// object slots without allocating them.  Go though the objects &
// build the free list, then set the apporpriate globals Don't call
// this function if you don't know what you're doing.
void special_reset_objects(void);

// attaches an object, such as a fireball, to another object, such as
// a robot
void obj_attach(dxxobject *parent,dxxobject *sub);

void create_small_fireball_on_object(dxxobject *objp, fix size_scale, int sound_flag);

// returns object number
objnum_t drop_marker_object(vms_vector *pos, segnum_t segnum, vms_matrix *orient, int marker_num);

void wake_up_rendered_objects(dxxobject *gmissp, int window_num);

void reset_player_object(void);

extern void object_rw_swap(object_rw *obj_rw, int swap);
dxxobject *obj_find_first_of_type (int);
void dead_player_frame(void);

#ifdef NETWORK
void fuelcen_check_for_goal (segment *);
#endif

extern int Drop_afterburner_blob_flag;

#endif

#endif
