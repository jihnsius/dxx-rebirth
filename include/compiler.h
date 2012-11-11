#ifndef _COMPILER_H
#define _COMPILER_H

#ifdef __GNUC__
#define __noreturn __attribute__ ((noreturn))
#define __attribute_gcc_format(X) __attribute__ ((format X))
#define __attribute_nonnull __attribute__((nonnull))
#define __attribute_malloc __attribute__((malloc))
#define __attribute_warn_unused_result __attribute__((warn_unused_result))
#define __attribute_used __attribute__((used))
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
#define __attribute_nonnull
#define __attribute_malloc
#define __attribute_warn_unused_result
#define __attribute_used
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

#ifdef __cplusplus
#define __apply_c_linkage	extern "C"
#else
#define __apply_c_linkage
#endif

#if defined(__GNUC__) && defined(__OPTIMIZE__)
#ifdef __INLINE__
#define define_interposition_check(A,C,B)	\
	__errordecl(__trap_bad_argument_##A##_##B, #A #C #B);	\
	if (__builtin_constant_p(A C B) && A C B) __trap_bad_argument_##A##_##B()
#else
#define define_interposition_check(A,C,B)
#endif
#define CHK_REDIRECT(R,N,A,B)	\
	__apply_c_linkage R (unchecked_##N) A;	\
	__extern_always_inline R (N) A;	\
	__extern_always_inline R (N) A	\
	{ B; }
#else
#define CHK_REDIRECT(R,N,A,B)	__apply_c_linkage R unchecked_##N A
#endif

#endif
