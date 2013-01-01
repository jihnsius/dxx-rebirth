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
 * Info on canvases, screens, etc.
 *
 */


#ifndef _SCREEN_H
#define _SCREEN_H

#include "gr.h"

#ifdef __cplusplus

// What graphics modes the game & editor open

// for Screen_mode variable
#define SCREEN_MENU		0	// viewing the menu screen
#define SCREEN_GAME		1	// viewing the menu screen
#define SCREEN_EDITOR		2	// viewing the editor screen
#define SCREEN_MOVIE		3	// viewing a movie

//from game.c
int set_screen_mode(int sm);	// True = editor screen

//About the screen

extern grs_canvas		Screen_3d_window;		// The rectangle for rendering the mine to
extern grs_canvas		*VR_offscreen_buffer;		// The offscreen data buffer
extern grs_canvas		VR_render_buffer[2];		// Two offscreen buffers for left/right eyes.
extern grs_canvas		VR_render_sub_buffer[2];	// Two sub buffers for left/right eyes.
extern grs_canvas		VR_screen_sub_pages[2];		// Two sub pages of VRAM if paging is available

void game_init_render_buffers(int render_max_w, int render_max_h);
void set_display_mode(int mode);
extern int Default_display_mode;

#define MENU_SCREEN_MODE (menu_use_game_res?Game_screen_mode:MENU_HIRES_MODE)

#endif

#endif /* _SCREENS_H */
