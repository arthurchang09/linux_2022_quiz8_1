a.out: test.c memchr.S macros.h
		gcc -m64 -g -ggdb -Wextra test.c memchr.S -o a.out