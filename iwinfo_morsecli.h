/*
 * iwinfo - Wireless Information Library - morsecli headers
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

#ifndef __IWINFO_MORSECLI_H_
#define __IWINFO_MORSECLI_H_

#define morse_cli_stats_query(ifname, ...) \
	__morse_cli_stats_query(ifname, ##__VA_ARGS__, NULL)

int __morse_cli_stats_query(const char *ifname, ...);

#endif