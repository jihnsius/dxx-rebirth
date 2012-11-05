/* $Id: collide.h,v 1.1.1.1 2006/03/17 19:54:41 zicodxx Exp $ */
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
 * Header for collide.c
 *
 */


#ifndef _COLLIDE_H
#define _COLLIDE_H

#include "playsave.h"

void collide_init();
void collide_two_objects(dxxobject * A, dxxobject * B, vms_vector *collision_point);
void collide_object_with_wall(dxxobject * A, fix hitspeed, short hitseg, short hitwall, vms_vector * hitpt);
extern void apply_damage_to_player(dxxobject *player, dxxobject *killer, fix damage, ubyte possibly_friendly);

// Returns 1 if robot died, else 0.
extern int apply_damage_to_robot(dxxobject *robot, fix damage, int killer_objnum);

extern fix Boss_invulnerable_dot;

#define PERSISTENT_DEBRIS (PlayerCfg.PersistentDebris && !(Game_mode & GM_MULTI)) // no persistent debris in multi

extern void collide_player_and_materialization_center(dxxobject *objp);
extern void collide_robot_and_materialization_center(dxxobject *objp);

extern void scrape_player_on_wall(dxxobject *obj, short hitseg, short hitwall, vms_vector * hitpt);

extern void collide_player_and_nasty_robot(dxxobject * player, dxxobject * robot, vms_vector *collision_point);

extern void net_destroy_controlcen(dxxobject *controlcen);
extern void collide_player_and_powerup(dxxobject * player, dxxobject * powerup, vms_vector *collision_point);
extern int check_effect_blowup(segment *seg,int side,vms_vector *pnt, dxxobject *blower, int force_blowup_flag);
extern void apply_damage_to_controlcen(dxxobject *controlcen, fix damage, short who);
extern void bump_one_object(dxxobject *obj0, vms_vector *hit_dir, fix damage);
void do_final_boss_hacks();

//see if wall is volatile, and if so, cause damage to player
//returns true if player is in lava
int check_volatile_wall(dxxobject *obj,int segnum,int sidenum,vms_vector *hitpt);

#endif /* _COLLIDE_H */
