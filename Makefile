a.out : test.c memchr.h memchr.c macros.h memchr.S strchr.c strchr.S
		gcc -m64 test.c memchr.c  memchr.S strchr.c strchr.S -o a.out
