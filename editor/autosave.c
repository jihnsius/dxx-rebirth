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
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Autosave system:
 * Saves current mine to disk to prevent loss of work, and support undo.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "dxxerror.h"

#include "inferno.h"
#include "editor.h"
#include "u_mem.h"
#include "ui.h"
#include "strutil.h"

#define AUTOSAVE_PERIOD 5			// Number of minutes for timed autosave

int		Autosave_count;
int		Autosave_numfiles;
int		Autosave_total;
int		undo_count;
int		original;

int		Timer_save_flag=0;
int		Autosave_flag;

char		undo_status[10][100];

void init_autosave(void) {
//    int i;

    Autosave_count = 0;
    Autosave_numfiles = 0;
	 Autosave_flag = 0;
    undo_count = 0;
    //MALLOC( undo_status, char *, 10 );
    //for (i=0; i<10; i++)
    //    MALLOC( undo_status[i], char, 100 );
    autosave_mine(mine_filename);
}

void close_autosave(void) {
    int i;
    char *delname, *ext;

    for (i=0;i<Autosave_total;i++) {

	MALLOC(delname, char, PATH_MAX);

        strcpy ( delname, mine_filename );
        strupr( delname );
	if ( !strcmp(delname, "*.MIN") ) strcpy(delname, "TEMP.MIN");

        ext = strstr(delname, ".MIN");
        sprintf( ext, ".M%d", i );

        remove( delname );
        d_free( delname );
    }
    //for (i = 0; i < 10; i++) d_free( undo_status[i] );
    //d_free( undo_status );

}

void autosave_mine(char *name) {
    char *savename, *ext;

	if (Autosave_flag) {
	
	    MALLOC(savename, char, PATH_MAX);

	
	    strcpy ( savename, name );
	    strupr( savename );
	    if ( !strcmp(savename, "*.MIN") ) strcpy(savename, "TEMP.MIN");
	
	    ext = strstr(savename, ".MIN");
	    sprintf( ext, ".M%d", Autosave_count );
	
	    med_save_mine( savename );
	    Autosave_count++;
	    if (undo_count > 0) undo_count--;
	    if (Autosave_count > 9) Autosave_count -= 10;
	    if (Autosave_numfiles < 10)
	        Autosave_numfiles++;
	    if (Autosave_total < 10)
	        Autosave_total++;
	
	    d_free(savename);

	}

}


void print_clock( int seconds, char message[10] ) {
	int w,h,aw;
	char	*p;

	//	Make colon flash
	if (seconds & 1)
		if ((p = strchr(message, ':')) != NULL)
			*p = ' ';

	gr_set_current_canvas( NULL );
	gr_set_fontcolor( CBLACK, CGREY );
	gr_get_string_size( message, &w, &h, &aw );
	gr_setcolor( CGREY );
	gr_rect( 700, 0, 799, h+1 );
	gr_string( 700, 0, message );
	gr_set_fontcolor( CBLACK, CWHITE );
}

struct tm Editor_time_of_day;

void set_editor_time_of_day()
{
	time_t	 ltime;

	time( &ltime );
	Editor_time_of_day = *localtime( &ltime );
}

#if 0
void TimedAutosave(char *) 
{
	int 		 month,day,hour,minute,second;

	minute = Editor_time_of_day.tm_min;

	//if (second!=save_second)
	{
	}
	

#ifndef DEMO
	if (minute%AUTOSAVE_PERIOD != 0)
		Timer_save_flag = 1;

	if ((minute%AUTOSAVE_PERIOD == 0) && (Timer_save_flag)) {
		time_t	 ltime;

		autosave_mine(name);
		Timer_save_flag = 0;
		time( &ltime );
   	diagnostic_message("Mine Autosaved at %s\n", ctime(&ltime));
	}
#endif

}
#endif


int undo( void ) {
	Int3();
	return 2;
}

