#include "strchr.h"

char *strchr_linux_org(const char *s, int c)
{
	for (; *s != (char)c; ++s)
		if (*s == '\0')
			return NULL;
	return (char *)s;
}

/*char *strchr_x86(const char *s, int c)
{
	int d0;
	char *res;
	asm volatile("movb %%al,%%ah\n"
		"1:\tlodsb\n\t"
		"cmpb %%ah,%%al\n\t"
		"je 2f\n\t"
		"testb %%al,%%al\n\t"
		"jne 1b\n\t"
		"mov $1,%1\n"
		"2:\tmov %1,%0\n\t"
		"dec %0"
		: "=a" (res), "=&S" (d0)
		: "1" (s), "0" (c)
		: "memory");
	return res;
}*/

/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Based on strlen implementation by Torbjorn Granlund (tege@sics.se),
   with help from Dan Sahlin (dan@sics.se) and
   bug fix and commentary by Jim Blandy (jimb@ai.mit.edu);
   adaptation to strchr suggested by Dick Karpinski (dick@cca.ucsf.edu),
   and implemented by Roland McGrath (roland@ai.mit.edu).
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

char *strchr_glibc(const char *s, int c_in)
{
	const unsigned char *char_ptr;
	const unsigned long int *longword_ptr;
	unsigned long int longword, magic_bits, charmask;
	unsigned register char c;
	c = (unsigned char) c_in;
	for (char_ptr = (const unsigned char *) s;
		((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0;
		++char_ptr)
		if (*char_ptr == c)
		return (char *) char_ptr;
		else if (*char_ptr == '\0')
		return NULL;
	longword_ptr = (unsigned long int *) char_ptr;
	switch (sizeof (longword))
		{
		case 4: magic_bits = 0x7efefeffL; break;
		case 8: magic_bits = ((0x7efefefeL << 16) << 16) | 0xfefefeffL; break;
		default:
		abort ();
		}
	charmask = c | (c << 8);
	charmask |= charmask << 16;
	if (sizeof (longword) > 4)
		charmask |= (charmask << 16) << 16;
	if (sizeof (longword) > 8)
		abort ();
	for (;;)
	{
		longword = *longword_ptr++;
		if ((((longword + magic_bits)
			^ ~longword)
		& ~magic_bits) != 0 ||
		((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
		& ~magic_bits) != 0)
		{
		const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);
		if (*cp == c)
			return (char *) cp;
		else if (*cp == '\0')
			return NULL;
		if (*++cp == c)
			return (char *) cp;
		else if (*cp == '\0')
			return NULL;
		if (*++cp == c)
			return (char *) cp;
		else if (*cp == '\0')
			return NULL;
		if (*++cp == c)
			return (char *) cp;
		else if (*cp == '\0')
			return NULL;
		if (sizeof (longword) > 4)
			{
			if (*++cp == c)
			return (char *) cp;
			else if (*cp == '\0')
			return NULL;
			if (*++cp == c)
			return (char *) cp;
			else if (*cp == '\0')
			return NULL;
			if (*++cp == c)
			return (char *) cp;
			else if (*cp == '\0')
			return NULL;
			if (*++cp == c)
			return (char *) cp;
			else if (*cp == '\0')
			return NULL;
			}
		}
    }
  return NULL;
}