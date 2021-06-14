/*
   tmi.c - twitch irc library for C
   Copyright (C) 2021  Kacper Kocot <kocotian@kocotian.pl>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#ifndef _STR_H
#define _STR_H

#include <ctype.h>
#include <sys/types.h>
#include <string.h>

/* Types and pseudo-types */
typedef struct {
	char *data;
	size_t len;
} String;

#define Array(TYPE) struct { TYPE *data; size_t len; }

/* String functions */
String toString(char *s);
int Strcmp(String a, String b);
int Strcmpc(String s1, char *s2);
ssize_t Strtok(String *i, String *o, char c);
String Striden(String string);
String Strtrim(String str);

/* Array functions */
int _inArray(char *data, size_t len, void *val, size_t vlen);
#define inArray(ARR, VAL) (_inArray( \
				   (void *)(ARR).data, (ARR).len, &(VAL), sizeof (VAL)))
void *_prepareArray(void *data, size_t siz);
#define prepareArray(ARR) (_prepareArray(&(ARR), sizeof (ARR)))

/* Vector - dynamic Array */
#define newVector(ARR) ((ARR).data = malloc((ARR).len = 0))
#define pushVector(ARR, VAL) (((ARR).data = \
			realloc((ARR).data, \
				++((ARR).len) * (sizeof *((ARR).data)))), \
		(ARR).data[(ARR).len - 1] = (VAL))

#endif
