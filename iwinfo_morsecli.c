/*
 * iwinfo - Wireless Information Library - morsecli interface
 *
 *   Copyright (C) 2023 Morse Micro
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* popen using exec (rather than like system()).
 *
 * Since I have an irrational dislike of using /bin/sh unnecessarily,
 * and we're potentially calling this quite a lot :(
 */
static FILE *execvp_popen_read(const char *file, const char *const argv[]) 
{
	int pipefd[2];

	if (pipe(pipefd)) return NULL;

	switch (fork())
	{
	case 0: /* child */
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		execvp(file, (char * const*)argv);
		exit(1);
		break;
	default: /* parent */
		close(pipefd[1]);
		return fdopen(pipefd[0], "r");
	case -1:
		close(pipefd[1]);
		return NULL;
	}
}

/* Query particular stats from morse_cli.
 *
 * Style of function echos nl80211_hostapd_query.
 *
 * This is the YAGNI version of this, since hopefully
 * it will be replaced by netlink queries in time.
 *
 * I don't use morse_cli's filter, since it's hard to construct
 * a regex covering all the possible options (and filter
 * doesn't do anything smart and still queries all the stats,
 * so we may as well not use the regex engine at all).
 *
 * Notably:
 *  - uses the plain text output rather than JSON
 *    (which means it can't handle nested stats)
 *  - only handles integer stats
 */
int __morse_cli_stats_query(const char *ifname, ...)
{
	va_list ap, ap_cur;
	int *dest;
	char *search, *key, *val, buf[128];
	int found = 0;
	FILE *fp;
	/* For now, the only stat we want is noise,
	 * so restrict to PHY core.
	 */
	const char *const argv[] = {"morse_cli", "-i", ifname, "stats", "-u", NULL};

	fp = execvp_popen_read("morse_cli", argv);

	if (!fp)
		return 0;

	va_start(ap, ifname);

	/* iterate applicable lines and copy found values into dest buffers */
	while (fgets(buf, sizeof(buf), fp))
	{
		key = strtok(buf, ":\n");
		val = strtok(NULL, "\n");

		if (!key || !val || !*key)
			continue;

		va_copy(ap_cur, ap);

		while ((search = va_arg(ap_cur, char *)) != NULL)
		{
			dest = va_arg(ap_cur, int *);

			if (!strcmp(search, key))
			{
				*dest = atoi(val);
				found++;
				break;
			}
		}

		va_end(ap_cur);
	}

	va_end(ap);

	fclose(fp);

	return found;
}