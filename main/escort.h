/*
 *
 * Header for escort.c
 *
 */

#ifndef _ESCORT_H
#define _ESCORT_H
#define GUIDEBOT_NAME_LEN 9

#ifdef __cplusplus
extern "C" {
#endif

extern void change_guidebot_name(void);
extern void do_escort_menu(void);
extern void detect_escort_goal_accomplished(int index);
extern void set_escort_special_goal(int key);
void init_buddy_for_level(void);
void invalidate_escort_goal(void);
void recreate_thief(dxxobject *objp);
void drop_stolen_items (dxxobject *);
extern fix64 Buddy_sorry_time;

#ifdef __cplusplus
}
#endif
#endif // _ESCORT_H
