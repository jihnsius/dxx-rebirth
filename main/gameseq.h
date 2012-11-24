/* $Id: gameseq.h,v 1.1.1.1 2006/03/17 19:57:54 zicodxx Exp $ */
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
 * Prototypes for functions for game sequencing.
 *
 */


#ifndef _GAMESEQ_H
#define _GAMESEQ_H

#include "player.h"
#include "mission.h"
#include "object.h"

#define SUPER_MISSILE       0
#define SUPER_SEEKER        1
#define SUPER_SMARTBOMB     2
#define SUPER_SHOCKWAVE     3

#define LEVEL_NAME_LEN 36       //make sure this is multiple of 4!

#ifdef __cplusplus
extern "C" {
#endif

// Current_level_num starts at 1 for the first level
// -1,-2,-3 are secret levels
// 0 means not a real level loaded
extern int Current_level_num, Next_level_num;
extern char Current_level_name[LEVEL_NAME_LEN];
extern obj_position Player_init[MAX_PLAYERS];


// This is the highest level the player has ever reached
extern int Player_highest_level;

//
// New game sequencing functions
//

// starts a new game on the given level
void StartNewGame(int start_level);

// starts the next level
void StartNewLevel(int level_num, int secret_flag);
void StartLevel(int random_flag);

// Actually does the work to start new level
void StartNewLevelSub(int level_num, int page_in_textures, int secret_flag);

void InitPlayerObject();            //make sure player's object set up
void init_player_stats_game(ubyte pnum);      //clear all stats

// called when the player has finished a level
// if secret flag is true, advance to secret level, else next normal level
void PlayerFinishedLevel(int secret_flag);

// called when the player has died
void DoPlayerDead(void);

// load just the hxm file
void load_level_robots(int level_num);

// load a level off disk. level numbers start at 1.
// Secret levels are -1,-2,-3
void LoadLevel(int level_num, int page_in_textures);

void gameseq_remove_unused_players();

void update_player_stats();

// from scores.c

void show_high_scores(int place);
void draw_high_scores(int place);
int add_player_to_high_scores(player *pp);
void input_name (int place);
int reset_high_scores();
void init_player_stats_level(int secret_flag);

void open_message_window(void);
void close_message_window(void);

// create flash for player appearance
void create_player_appearance_effect(dxxobject *player_obj);

// goto whatever secrect level is appropriate given the current level
void goto_secret_level();

// reset stuff so game is semi-normal when playing from editor
void editor_reset_stuff_on_level();

// Show endlevel bonus scores
void DoEndLevelScoreGlitz(int network);

// stuff for multiplayer
extern int NumNetPlayerPositions;

void bash_to_shield(objnum_t, const char *);

int p_secret_level_destroyed(void);
void ExitSecretLevel(void);
void do_cloak_invul_secret_stuff(fix64 old_gametime);
void EnterSecretLevel(void);
void copy_defaults_to_robot(dxxobject *objp);
void init_player_stats_new_ship(ubyte pnum);
extern fix StartingShields;
extern int	First_secret_visit;
extern int Do_appearance_effect;

#ifdef __cplusplus
}
#endif

#endif /* _GAMESEQ_H */
