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

char *strchr_linux_org(const char *s, int c);
//char *strchr_x86(const char *s, int c);
char *strchr_glibc(const char *s, int c);
char *strchr_S(const char *s, int c);