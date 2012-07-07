/* fileio.c in /misc for d1x file reading */
#ifndef _STRIO_H
#define _STRIO_H

#include <physfs.h>
#include "compiler.h"

char* fgets_unlimited(PHYSFS_file *f) __attribute_warn_unused_result __attribute_malloc __attribute_nonnull;
char *splitword(char *s, char splitchar);

#endif
