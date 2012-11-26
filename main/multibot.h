/* $Id: multibot.h,v 1.1.1.1 2006/03/17 19:55:28 zicodxx Exp $ */
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
 * Header file for multiplayer robot support.
 *
 */


#ifndef _MULTIBOT_H
#define _MULTIBOT_H

#ifdef __cplusplus

#define MAX_ROBOTS_CONTROLLED 5

extern objnum_t robot_controlled[MAX_ROBOTS_CONTROLLED];
extern int robot_agitation[MAX_ROBOTS_CONTROLLED];
extern int robot_fired[MAX_ROBOTS_CONTROLLED];

int multi_can_move_robot(objnum_t objnum, int agitation);
void multi_send_robot_position(objnum_t objnum, int fired);
void multi_send_robot_fire(objnum_t objnum, int gun_num, vms_vector *fire);
void multi_send_claim_robot(objnum_t objnum);
void multi_send_robot_explode(objnum_t,objnum_t,char);
void multi_send_create_robot(int robotcen, objnum_t objnum, int type);
void multi_send_boss_actions(objnum_t bossobjnum, int action, int secondary);
static inline void multi_send_boss_teleport_action(objnum_t bossobjnum, segnum_t seg)
{
	multi_send_boss_actions(bossobjnum, 1, seg);
}
static inline void multi_send_boss_cloak_action(objnum_t bossobjnum)
{
	multi_send_boss_actions(bossobjnum, 2, 0);
}
int multi_send_robot_frame(int sent);

void multi_do_robot_explode(char *buf);
void multi_do_robot_position(char *buf);
void multi_do_claim_robot(char *buf);
void multi_do_release_robot(char *buf);
void multi_do_robot_fire(char *buf);
void multi_do_create_robot(char *buf);
void multi_do_boss_actions(char *buf);
void multi_do_create_robot_powerups(char *buf);

int multi_explode_robot_sub(objnum_t botnum, objnum_t killer,char);

void multi_drop_robot_powerups(objnum_t objnum);
void multi_dump_robots(void);

void multi_strip_robots(int playernum);
void multi_check_robot_timeout(void);

void multi_robot_request_change(dxxobject *robot, int playernum);

#endif

#endif /* _MULTIBOT_H */
