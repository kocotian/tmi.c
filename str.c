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

#include "str.h"

/* String functions */

String
toString(char *s)
{
	String ret;
	ret.len = strlen(ret.data = s);
	return ret;
}

int
Strcmp(String a, String b)
{
	if (a.len != b.len) return -1;
	return strncmp(a.data, b.data, a.len);
}

int
Strcmpc(String s1, char *s2)
{
	if (s1.len != strlen(s2)) return -1;
	return strncmp(s1.data, s2, s1.len);
}

ssize_t
Strtok(String *i, String *o, char c)
{
	ssize_t n;

	if (!i->len) return 0;

	*o = *i;

	for (n = 0; n < i->len; ++n) {
		if (i->data[n] == c) break;
	}

	o->len = n;
	if (n == i->len) {
		i->data += ++n;
		i->len = 0;
	} else {
		i->data += ++n;
		i->len -= n;
	}

	return n;
}

String
Striden(String str)
{
	size_t i;
	for (i = 0; ((str.data[i] >= 'a' && str.data[i] <= 'z')
				|| (str.data[i] >= 'A' && str.data[i] <= 'Z')
				|| (str.data[i] && str.data[i] >= '0' && str.data[i] <= '9'))
				&& (i < str.len); ++i);
	str.len = i;
	return str;
}

String
Strtrim(String str)
{
	size_t i = 0;
	while (isspace(*(str.data)) && i < str.len) {
		++str.data;
		--str.len;
		++i;
	}
	while (isspace(*(str.data + (str.len - 1))))
		--str.len;
	return str;
}

/* Array functions */
int
_inArray(char *data, size_t len, void *val, size_t vlen)
{
	size_t i, n;
	for (n = i = 0; i < len; ++i, data += vlen)
		if (!memcmp(data, val, vlen)) ++n;
	return (int)n;
}

void *
_prepareArray(void *data, size_t siz)
{
	return memset(data, 0, siz);
}
