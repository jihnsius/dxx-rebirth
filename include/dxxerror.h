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
 * Header for error handling/printing/exiting code
 *
 */

#ifndef _ERROR_H
#define _ERROR_H

#include <stdio.h>
#include "compiler.h"

int error_init(void (*func)(const char *), const char *fmt,...);    //init error system, set default message, returns 0=ok
void set_exit_message(const char *fmt,...) __attribute_gcc_format((printf, 1, 2));	//specify message to print at exit
void Warning(const char *fmt,...) __attribute_gcc_format((printf, 1, 2));				//print out warning message to user
void set_warn_func(void (*f)(const char *s));//specifies the function to call with warning messages
void clear_warn_func(void (*f)(const char *s));//say this function no longer valid
void _Assert(int expr,const char *expr_text,const char *filename,int linenum);	//assert func
void Error(const char *func, unsigned line, const char *fmt,...) __noreturn __attribute_gcc_format((printf, 3, 4));				//exit with error code=1, print message
#define Error(F,...)	((Error)(__func__, __LINE__, (F), ## __VA_ARGS__))
void Assert(int expr);
void Int3();
#ifndef NDEBUG		//macros for debugging

# if defined(__APPLE__) || defined(macintosh)
extern void Debugger(void);	// Avoids some name clashes
#  define Int3 Debugger
# else
#  define Int3() ((void)0)
# endif // Macintosh
#define Assert(expr) ((expr)?(void)0:(void)_Assert(0,#expr,__FILE__,__LINE__))

#else					//macros for real game

//Changed Assert and Int3 because I couldn't get the macros to compile -KRB
#define Assert(__ignore) ((void)0)
#define Int3() ((void)0)
#endif

#endif /* _ERROR_H */
