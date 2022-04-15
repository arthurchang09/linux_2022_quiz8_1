#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

/* Nonzero if either X or Y is not aligned on a "long" boundary */
#define UNALIGNED(X) ((long) X & (sizeof(long) - 1))

/* How many bytes are loaded each iteration of the word copy loop */
#define LBLOCKSIZE (sizeof(long))

/* Threshhold for punting to the bytewise iterator */
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

#if LONG_MAX == 2147483647L
#define DETECT_NULL(X) (((X) -0x01010101) & ~(X) & 0x80808080)
#else
#if LONG_MAX == 9223372036854775807L
/* Nonzero if X (a long int) contains a NULL byte. */
#define DETECT_NULL(X) (((X) -0x0101010101010101) & ~(X) & 0x8080808080808080)
#else
#error long int is not a 32bit or 64bit type.
#endif
#endif

/* @return nonzero if (long)X contains the byte used to fill MASK. */
#define DETECT_CHAR(X, MASK) (DETECT_NULL(X ^ MASK))


void *memchr_org(const void *s, int c, size_t n);
void *memchr_opt(const void *src_void, int c, size_t length);
void *memchr_x86(const void *cs, int c, size_t count);
void *memchr1(const void *cs, int c, size_t count);
void *memchr_glibc(const void *cs, int c, size_t count);