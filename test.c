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

#define LENGTH 120
#define CUT 10
#define STRLEN 1000

void *memchr_org(const void *s, int c, size_t n)
{
    const unsigned char *p = s;
    while (n-- != 0) {
        if ((unsigned char)c == *p++) {
            return (void *)(p - 1);
        }
    }
    return NULL;
}


void *memchr_opt(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *) src_void;
    unsigned char d = c;
    while (UNALIGNED(src)) {
        if (!length--)
            return NULL;
        if (*src == d)
            return (void *) src;
        src++;
    }
    if (!TOO_SMALL(length)) {
        /* If we get this far, we know that length is large and
         * src is word-aligned.
         */

        /* The fast code reads the source one word at a time and only performs
         * the bytewise search on word-sized segments if they contain the search
         * character, which is detected by XORing the word-sized segment with a
         * word-sized block of the search character and then detecting for the
         * presence of NULL in the result.
         */
        unsigned long *asrc = (unsigned long *) src;
        unsigned long mask = d << 8 | d;
        mask = mask << 16 | mask;
        for (unsigned int i = 32; i < LBLOCKSIZE * 8; i <<= 1)
            mask = (mask << i) | mask;
        int index = 0;
        while (length >= LBLOCKSIZE) {
            /* XXXXX: Your implementation should appear here */
            if ( !DETECT_CHAR(*asrc, mask)) {
                asrc += 1;
                length -= LBLOCKSIZE;
            }
            else
                break;
        }

        /* If there are fewer than LBLOCKSIZE characters left, then we resort to
         * the bytewise loop.
         */
        src = (unsigned char *) asrc;
    }

    while (length--) {
        if (*src == d)
            return (void *) src;
        src++;
    }
    return NULL;
}

void *memchr_x86(const void *cs, int c, size_t count)
{
	int d0;
	void *res;
	if (!count)
		return NULL;
	asm volatile("repne\n\t"
		"scasb\n\t"
		"je 1f\n\t"
		"mov $1,%0\n"
		"1:\tdec %0"
		: "=D" (res), "=&c" (d0)
		: "a" (c), "0" (cs), "1" (count)
		: "memory");
	return res;
}

int cmp ( const void *a , const void *b ) 
{ 
    return *(int *)a > *(int *)b;
}

void *memchr1(const void *cs, int c, size_t count);

int main()
{   
    /*const char str[] = "http://wiki.csie.ncku.edu.tw";
    const char ch = 'h';
    struct timespec t1,t2;
    clock_gettime(CLOCK_REALTIME, &t1);
    char *ret = memchr1(str, ch, strlen(str));
    clock_gettime(CLOCK_REALTIME, &t2);
    printf("time opt %ld\n", t2.tv_nsec - t1.tv_nsec);
    printf("String after |%c| is - |%s|\n", ch, ret);
    return 0;*/

    /*char str[32768];
    memset(str,'0', sizeof(char)*1024);
    str[32767] = '\0';
    str[32765] = '4';
    char ch = '4';*/
    
    char str[STRLEN];
    memset(str,'0', sizeof(char)*STRLEN);
    str[STRLEN - 1] = '\0';
    //str[11] = '4';
    char ch = '4';


    int data[4][LENGTH];
    memset(data,0,sizeof(data));
    int sum[4];
    memset(sum,0,sizeof(sum));
    char *ret = NULL;

    struct timespec t1, t2, t3, t4, t5, t6, t7, t8;

    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));
        for (int i = 0; i < LENGTH; ++i) {

            clock_gettime(CLOCK_REALTIME, &t1);
            ret = memchr_opt(str, ch, strlen(str));
            clock_gettime(CLOCK_REALTIME, &t2);
            //printf("time opt %ld\n", t2.tv_nsec - t1.tv_nsec);
            //printf("String after |%c| is - |%s|\n", ch, ret);
            data[0][i] = t2.tv_nsec - t1.tv_nsec;

            usleep(1);

            clock_gettime(CLOCK_REALTIME, &t3);
            ret = memchr_x86(str, ch, strlen(str));
            clock_gettime(CLOCK_REALTIME, &t4);
            //printf("time x86 %ld\n", t4.tv_nsec - t3.tv_nsec);
            //printf("String after |%c| is - |%s|\n", ch, ret);
            data[1][i] = t4.tv_nsec - t3.tv_nsec;

            usleep(1);

            clock_gettime(CLOCK_REALTIME, &t5);
            ret = memchr_org(str, ch, strlen(str));
            clock_gettime(CLOCK_REALTIME, &t6);
            //printf("time org %ld\n", t6.tv_nsec - t5.tv_nsec);
            //printf("String after |%c| is - |%s|\n", ch, ret);
            data[2][i] = t6.tv_nsec - t5.tv_nsec;

            usleep(1);

            clock_gettime(CLOCK_REALTIME, &t7);
            ret =memchr1(str, ch, strlen(str));
            clock_gettime(CLOCK_REALTIME, &t8);
            //printf("time org %ld\n", t6.tv_nsec - t5.tv_nsec);
            //printf("String after |%c| is - |%s|\n", ch, ret1);
            data[3][i] = t8.tv_nsec - t7.tv_nsec;

            usleep(1);
        }
        printf("%d ",k);
        for (int i = 0; i < 4; ++i) {
            qsort(data[i],LENGTH,sizeof(int),cmp);
            for (int j = CUT; j < LENGTH - CUT; ++j) {
                sum[i] += data[i][j];
            }
            printf("%d ", sum[i] / (LENGTH - (CUT << 1)));
        }
        printf("\n");
    }
    return 0;
}