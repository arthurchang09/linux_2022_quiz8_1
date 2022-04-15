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
#include "memchr.h"
#include "strchr.h"

#define LENGTH 200
#define CUT 10
#define STRLEN 1000

#define BENCH(num, tdata, fun, str, ...) \
struct timespec ts1##num, ts2##num; \
for (int k = 0; k < STRLEN - 1; ++k){  \
    memset(str,'0', sizeof(char)*STRLEN);\
    str[STRLEN - 1] = '\0';\
    str[k] = '4';\
    memset(data,0,sizeof(data));\
    memset(sum,0,sizeof(sum)); \
    clock_gettime(CLOCK_REALTIME, &ts1##num);\
    for (int i = 0; i < LENGTH; ++i) { \
        char *ret = fun(str, __VA_ARGS__);\
    }\
    clock_gettime(CLOCK_REALTIME, &ts2##num);\
    tdata[num][k] = (ts2##num.tv_nsec - ts1##num.tv_nsec) / LENGTH;\
}\

int cmp ( const void *a , const void *b ) 
{ 
    return *(int *)a > *(int *)b;
}

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
    struct timespec t[10];

    long time_data[15][STRLEN - 1];
    
    /*for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr_opt(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);
        time_data[0][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }
    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr_x86(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);

        time_data[1][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }

    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr_org(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);

        time_data[2][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }

    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr1(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);

        time_data[3][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }

    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr_glibc(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);

        time_data[4][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }

    for (int k = 0; k < STRLEN - 1; ++k){
        memset(str,'0', sizeof(char)*STRLEN);
        str[STRLEN - 1] = '\0';
        str[k] = '4';
        memset(data,0,sizeof(data));
        memset(sum,0,sizeof(sum));

        clock_gettime(CLOCK_REALTIME, &t[0]);
        for (int i = 0; i < LENGTH; ++i) {
            char *ret = memchr(str, ch, strlen(str));
        }
        clock_gettime(CLOCK_REALTIME, &t[1]);

        time_data[5][k] = (t[1].tv_nsec - t[0].tv_nsec) / LENGTH;
    }*/
    BENCH(0, time_data, memchr_opt, str, ch, strlen(str));
    BENCH(1, time_data, memchr_x86, str, ch, strlen(str));
    BENCH(2, time_data, memchr_org, str, ch, strlen(str));
    BENCH(3, time_data, memchr1, str, ch, strlen(str));
    BENCH(4, time_data, memchr_glibc, str, ch, strlen(str));
    BENCH(5, time_data, memchr, str, ch, strlen(str));
    BENCH(6, time_data, strchr_linux_org, str, ch);
    BENCH(7, time_data, strchr_glibc, str, ch);
    BENCH(8, time_data, strchr_S, str, ch);
    BENCH(9, time_data, strchr, str, ch);

    for (int k = 0; k < STRLEN - 1; ++k){
        printf("%d ", k);
        for (int i = 0; i < 10; ++i) {
            printf("%ld ", time_data[i][k]);
        }
        printf("\n");
    }
    return 0;
}