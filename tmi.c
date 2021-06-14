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

#include <tmi.h>

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef char Buffer[640 * 1024];

/* static functions */
static int
vwritef(int fd, int sending, const char *restrict fmt, va_list ap)
{
	Buffer buf;
	int s, n;

	s = vsnprintf(buf, sizeof buf, fmt, ap);
	n = write(fd, buf, s);
	if (sending)
		if (write(fd, "\r\n", 2) != 2)
			return -1;
	return n + 2;
}

inline static int
writef(int fd, const char *restrict fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vwritef(fd, 0, fmt, ap);
	va_end(ap);
	return n;
}

inline static int
writelnf(int fd, const char *restrict fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vwritef(fd, 1, fmt, ap);
	va_end(ap);
	return n;
}

/* global functions */
tmi_t *
tmi_init(void)
{
	static tmi_t t = {
		.fd = -1,
		.oauth = NULL,
		.username = NULL,
		.channel = NULL
	};
	return &t;
}

int
tmi_connect(tmi_t *connection)
{
	int err;
	struct sockaddr_in addr;
	struct hostent *host;

	assert((host = gethostbyname(hostname)) != NULL);

	if ((connection->fd = socket(addr.sin_family = AF_INET,
					SOCK_STREAM, 0)) < 0)
		return connection->fd;

	addr.sin_port = htons(port);
	addr.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

	if ((err = connect(connection->fd, (struct sockaddr *)&addr,
				sizeof addr)) < 0)
		return err;

	return 0;
}

int
tmi_handle(tmi_t *connection, int (handler)(tmi_t *, tmievent_t *))
{
	if (fork() != 0)
		return 0;

	Buffer buf;
	size_t len;
	ssize_t rb;
	int rflag;
	tmievent_t ev;

	do {
		len = 0;
		/* reading line loop */
		while ((len < sizeof buf)
		   &&  ((rb = read(connection->fd, buf + len, 1)) > 0)
		) {
			if (buf[len] == '\r')
				rflag = 1;
			else if (buf[len] == '\n' && rflag == 1) {
				--len;
				break;
			} else
				rflag = 0;
			++len;
		}

		/* parsing line */
		ev.rawline.data = buf;
		ev.rawline.len = (signed)len;

		ev.params = ev.rawline;

		if (*buf == ':') {
			++ev.params.data;
			--ev.params.len;
			Strtok(&ev.params, &ev.prefix, ' ');
		} else {
			ev.prefix.data = ev.rawline.data;
			ev.prefix.len = 0;
		}

		Strtok(&ev.params, &ev.command, ' ');

		/* calling handler to handle parsed line */
		if (fork() == 0) {
			/* forking - asynchronous event handling :) */
			exit(handler(connection, &ev));
		}
	} while (rb);

	exit(0);
}

int
tmi_join(tmi_t *connection, char *oauth, char *username, char *channel)
{
	Buffer buf;
	int fd = connection->fd;

	/* TODO: write this in better way */
	assert(writelnf(fd, "CAP REQ :twitch.tv/membership") > 0);
	assert(writelnf(fd, "CAP REQ :twitch.tv/commands") > 0);
	read(fd, buf, sizeof buf);
	assert(writelnf(fd, "PASS %s", connection->oauth = oauth) > 0);
	assert(writelnf(fd, "USER %s %s %s :%s",
				username, username, username, username) > 0);
	assert(writelnf(fd, "NICK %s", connection->username = username) > 0);
	read(fd, buf, sizeof buf);
	assert(writelnf(fd, "JOIN #%s", connection->channel = channel) > 0);
	read(fd, buf, sizeof buf);

	return 0;
}

int
tmi_msg(tmi_t *connection, const char *fmt, ...)
{
	va_list ap;
	int n;
	int fd = connection->fd;
	n = writef(fd, "PRIVMSG #%s :", connection->channel);
	va_start(ap, fmt);
	n += writelnf(fd, fmt, ap);
	va_end(ap);
	return n;
}

tmiusername_t
tmi_ev_username(tmievent_t *ev)
{
	String username, tmp;
	if (!ev->prefix.len)
		return ev->prefix;
	tmp = ev->prefix, Strtok(&tmp, &username, '!');
	tmp = username, Strtok(&tmp, &username, '@');
	return username;
}

tmiprivmsg_t
tmi_ev_privmsg(tmievent_t *ev)
{
	tmiprivmsg_t privmsg;
	String tmp;
	if (!tmi_cmdcmp(ev, PRIVMSG)) {
		privmsg.contents.data = NULL;
		return privmsg;
	}
	privmsg.event = *ev;
	privmsg.contents = ev->params, Strtok(&privmsg.contents, &privmsg.channel, ' ');
	if (*privmsg.contents.data == ':')
		++privmsg.contents.data, --privmsg.contents.len;
	if (*privmsg.channel.data == '#')
		++privmsg.channel.data, --privmsg.channel.len;
	return privmsg;
}
