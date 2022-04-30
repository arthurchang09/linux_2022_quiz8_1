#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MEM_DEV "/dev/memchr"
 
int main()
{
    long long sz;

    char buf[1];
    //char write_buf[] = "testing_writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(MEM_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    /*for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " MEM_DEV ", returned the sequence %lld\n", sz);
    }*/
    //lseek(fd, '1', SEEK_SET);
    /*char buf1[15];
    memset(buf1, '0', sizeof(buf1));
    buf1[9] = '4';
    sz = read(fd, buf1, '4');
    printf("%s\n", buf1);
    printf("%lld\n", sz);*/

    char str[1000];
    memset(str, '0', sizeof(str));
    //str[899] = '\0';
    /*str[99] = '4';
    for (int i = 0; i < 900; ++i) {
        if (i == 9 ) {
            str[i] = '4';
        }
        else {
            str[i]= '2';
        }
    }*/
    str[0] = '4';
    //printf("%c\n", buff[0]);
    //printf("%d\n",buff);
    char ch = '4';
    
    //printf("%s\n",str);
    /*char buff[11]="1234567890";
    buff[10] = '\0';
    sz = write(fd, str, strlen(str));
    memset(str,0,strlen(str));
    sz = read(fd, str, '4');
    printf("return %s\n", str);
    printf("hi\n");*/
    //printk(KERN_INFO "Memchr Message: %s\n", ret);printf("%s\n",buff);
    for (int i = 0; i < 1000; ++i) {
        memset(str, '0', sizeof(str));
        //str[999] = '\0';
        str[i] = '4';
        sz = write(fd, str, strlen(str));
        memset(str,0,strlen(str));
        sz = read(fd, str, '4');
        printf("%d %lld\n", i, sz);
        //printf("%s\n", str);
        //printf("%ld\n", strlen(str));
        if (strlen(str) != 1000 - i) {
            printf("ERROR\n");
        }
        //break;
    }
    /*for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " MEM_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }*/


    close(fd);
    return 0;
}