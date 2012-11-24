/*
 *
 * Common types and defines
 *
 */

#ifndef _TYPES_H
#define _TYPES_H

#ifndef macintosh
#include <sys/types.h>
#endif

#include <limits.h>

// define a dboolean
typedef int dboolean;

//define a signed byte
typedef signed char sbyte;

//define unsigned types;
typedef unsigned char ubyte;
#if defined(_WIN32) || defined(macintosh)
typedef unsigned short ushort;
typedef unsigned int uint;
#endif

#if defined(_WIN32) || defined(__sun__) // platforms missing (u_)int??_t
# include <SDL/SDL_types.h>
#elif defined(macintosh) // misses (u_)int??_t and does not like SDL_types.h
# include <MacTypes.h>
 typedef SInt16 int16_t;
 typedef SInt32 int32_t;
 typedef SInt64 int64_t;
 typedef UInt16 u_int16_t;
 typedef UInt32 u_int32_t;
 typedef UInt64 u_int64_t;
#endif // macintosh
#if defined(_WIN32) || defined(__sun__) // platforms missing u_int??_t
 typedef Uint16 u_int16_t;
 typedef Uint32 u_int32_t;
 typedef Uint64 u_int64_t;
#endif // defined(_WIN32) || defined(__sun__)

#ifdef _MSC_VER
# include <stdlib.h> // this is where min and max are defined
#endif
#ifndef __cplusplus
#ifndef min
#define min(a,b) (((a)>(b))?(b):(a))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef bool
//define a boolean
typedef ubyte bool;
#endif

#ifndef NULL
#define NULL 0
#endif
#endif

// the following stuff has nothing to do with types but needed everywhere,
// and since this file is included everywhere, it's here.
#if  defined(__i386__) || defined(__ia64__) || defined(_WIN32) || \
(defined(__alpha__) || defined(__alpha)) || \
defined(__arm__) || defined(ARM) || \
(defined(__mips__) && defined(__MIPSEL__)) || \
defined(__SYMBIAN32__) || \
defined(__x86_64__) || \
defined(__LITTLE_ENDIAN__)	// from physfs_internal.h
//# define WORDS_BIGENDIAN 0
#else
# define WORDS_BIGENDIAN 1
#endif

#include "compiler.h"

#ifndef PACKAGE_STRING
# define PACKAGE_STRING "d2x"
#endif

#define DECLARE_TYPESAFE_INDEX(B,N,V)	\
	namespace typesafe_idx_##B { struct N##_t; enum { N = V }; }	\
	void B##_##N(typesafe_idx_##B::N##_t);	\
	typedef void (*B##_##N##_type_t)(typesafe_idx_##B::N##_t)
#define DEFINE_CONSTRUCT_SPECIAL(C,T)	\
	constexpr C(T##_type_t) : contained_value(T) {}	\
	C & operator=(T##_type_t) { contained_value = T; return *this; }
#define DEFINE_COMPARE_SPECIAL(OP,T)	bool operator OP(T##_type_t) const { return (contained_value OP T); }
#define DEFINE_COMPARE_PASSTHROUGH(OP,T)	bool operator OP(const T& t) const { return (contained_value OP t.contained_value); }

#endif //_TYPES_H

