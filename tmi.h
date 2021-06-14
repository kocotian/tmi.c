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

#ifndef _TMI_H
#define _TMI_H

#include <sys/types.h>

#include "str.h"

/* configuration */
static char *hostname           = "irc.twitch.tv";
static unsigned int port        = 6667;

/* types */
typedef struct twitch {
	int fd;
	char *oauth, *username, *channel;
} tmi_t;

typedef struct tmievent {
	String rawline;

	String prefix;
	String command;
	String params;
} tmievent_t;

/* functions */
tmi_t *tmi_init(void);
int tmi_connect(tmi_t *connection);
int tmi_handle(tmi_t *connection, int (handler)(tmi_t *, tmievent_t *));
int tmi_join(tmi_t *connection, char *oauth, char *username, char *channel);
int tmi_msg(tmi_t *connection, const char *fmt, ...);

typedef String tmiusername_t;
tmiusername_t tmi_ev_username(tmievent_t *ev);

typedef struct {
	tmievent_t event;
	String channel;
	String contents;
} tmiprivmsg_t;
tmiprivmsg_t tmi_ev_privmsg(tmievent_t *ev);

#define tmi_cmdcmp(EVPTR, CMD) (!Strcmpc((EVPTR)->command, #CMD))

#endif
