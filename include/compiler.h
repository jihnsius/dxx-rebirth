#ifndef _COMPILER_H
#define _COMPILER_H

#ifdef __GNUC__
#define __noreturn __attribute__ ((noreturn))
#define __attribute_gcc_format(X) __attribute__ ((format X))
#define __pack__ __attribute__((packed))
/*
 * The unaligned forms violate strict aliasing, which triggers a warning
 * in gcc 4.7 and above.
 */
#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || (__GNUC__ > 4)
#define COPY_WORDS_ALIGNED
#endif
#else
#define __noreturn
#define __attribute_gcc_format(X)
#if defined(_MSC_VER)
# pragma pack(push, packing)
# pragma pack(1)
# define __pack__
#elif defined(macintosh)
# pragma options align=packed
# define __pack__
#else
# error d2x will not work without packed structures
#endif
#endif

#ifdef _MSC_VER
# define inline __inline
#endif

#endif
