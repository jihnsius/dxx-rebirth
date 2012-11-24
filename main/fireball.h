/* $Id: fireball.h,v 1.1.1.1 2006/03/17 19:54:36 zicodxx Exp $ */
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
 * Header for fireball.c
 *
 */


#ifndef _FIREBALL_H
#define _FIREBALL_H

#include "object.types.h"

// explosion types
#define ET_SPARKS       0   //little sparks, like when laser hits wall
#define ET_MULTI_START  1   //first part of multi-part explosion
#define ET_MULTI_SECOND 2   //second part of multi-part explosion

#ifdef __cplusplus
extern "C" {
#endif

// data for exploding walls (such as hostage door)
typedef struct expl_wall {
	segnum_t segnum;
	int sidenum;
	fix time;
} expl_wall;

#define MAX_EXPLODING_WALLS     10

extern expl_wall expl_wall_list[MAX_EXPLODING_WALLS];

dxxobject *object_create_explosion(segnum_t segnum, vms_vector *position, fix size, int vclip_type);
dxxobject *object_create_muzzle_flash(segnum_t segnum, vms_vector *position, fix size, int vclip_type);

dxxobject *object_create_badass_explosion(dxxobject *objp, segnum_t segnum,
		vms_vector *position, fix size, int vclip_type,
		fix maxdamage, fix maxdistance, fix maxforce, int parent);

// blows up a badass weapon, creating the badass explosion
// return the explosion object
dxxobject *explode_badass_weapon(dxxobject *obj,vms_vector *pos);

// blows up the player with a badass explosion
// return the explosion object
dxxobject *explode_badass_player(dxxobject *obj);

void explode_object(dxxobject *obj,fix delay_time);
void do_explosion_sequence(dxxobject *obj);
void do_debris_frame(dxxobject *obj);      // deal with debris for this frame

void draw_fireball(dxxobject *obj);

void explode_wall(segnum_t segnum, int sidenum);
void do_exploding_wall_frame(void);
void init_exploding_walls(void);
extern void maybe_drop_net_powerup(int powerup_type);
extern void maybe_replace_powerup_with_energy(dxxobject *del_obj);

extern int get_explosion_vclip(dxxobject *obj, int stage);
int drop_powerup(object_type_t type, int id, int num, vms_vector *init_vel, vms_vector *pos, segnum_t segnum);

// creates afterburner blobs behind the specified object
void drop_afterburner_blobs(dxxobject *obj, int count, fix size_scale, fix lifetime);

/*
 * reads n expl_wall structs from a PHYSFS_file and swaps if specified
 */
extern void expl_wall_read_n_swap(expl_wall *ew, int n, int swap, PHYSFS_file *fp);
segnum_t pick_connected_segment(dxxobject *objp, int max_depth);
extern fix Flash_effect;

#ifdef __cplusplus
}
#endif

#endif /* _FIREBALL_H */
