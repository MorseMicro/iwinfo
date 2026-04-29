/*
 * Copyright 2022 Morse Micro
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dot11ah_channel.h"
#include "dot11ah_channel_rules.c"

static const channel_to_halow_freq_t kNullAhValue = {0, 0, 0, 0};

/* US map */
static const country_channel_map_t us_channel_map = {
	.country = "US",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(us_s1g_channels),
	.ah_vals = us_s1g_channels,
};

/* AU 802.11-REVmf (includes 802.11-2024 channels)
 *
 * We can share the same channel map because the unsupported
 * channels won't be returned when the driver is queried.
 */
static const country_channel_map_t au_revmf_channel_map = {
	.country = "AU",
	.chan_schemes = CHAN_SCHEME_80211_2024 | CHAN_SCHEME_80211_REVMF,
	.num_mapped_channels = ARRAY_SIZE(au_revmf_s1g_channels),
	.ah_vals = au_revmf_s1g_channels,
};

/* AU 802.11-2020 (legacy) map */
static const country_channel_map_t au_2020_channel_map = {
	.country = "AU",
	.chan_schemes = CHAN_SCHEME_80211_2020,
	.num_mapped_channels = ARRAY_SIZE(au_2020_s1g_channels),
	.ah_vals = au_2020_s1g_channels,
};

static const country_channel_map_t br_channel_map = {
	.country = "BR",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(br_s1g_channels),
	.ah_vals = br_s1g_channels,
};

/* NZ map */
static const country_channel_map_t nz_channel_map = {
	.country = "NZ",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(nz_s1g_channels),
	.ah_vals = nz_s1g_channels,
};

/* EU map */
static const country_channel_map_t eu_channel_map = {
	.country = "EU",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(eu_s1g_channels),
	.ah_vals = eu_s1g_channels,
};

/* GB map */
static const country_channel_map_t gb_channel_map = {
	.country = "GB",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(gb_s1g_channels),
	.ah_vals = gb_s1g_channels,
};

/* IN map */
static const country_channel_map_t in_channel_map = {
	.country = "IN",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(in_s1g_channels),
	.ah_vals = in_s1g_channels,
};

/* JP map */
static const country_channel_map_t jp_channel_map = {
	.country = "JP",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(jp_s1g_channels),
	.ah_vals = jp_s1g_channels,
};

/* KR map */
static const country_channel_map_t kr_channel_map = {
	.country = "KR",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(kr_s1g_channels),
	.ah_vals = kr_s1g_channels,
};

/* SG map */
static const country_channel_map_t sg_channel_map = {
	.country = "SG",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(sg_s1g_channels),
	.ah_vals = sg_s1g_channels,
};

static const country_channel_map_t ca_channel_map = {
	.country = "CA",
	.chan_schemes = CHAN_SCHEME_ALL,
	.num_mapped_channels = ARRAY_SIZE(ca_s1g_channels),
	.ah_vals = ca_s1g_channels,
};

static const country_channel_map_t channel_map_terminate = {
	.country = {0,0,0},
	.num_mapped_channels = 0,
	.ah_vals = NULL,
};

static const country_channel_map_t *mapped_channel[] = {
	&us_channel_map,
	&au_2020_channel_map,
	&au_revmf_channel_map,
	&br_channel_map,
	&nz_channel_map,
	&eu_channel_map,
	&gb_channel_map,
	&in_channel_map,
	&jp_channel_map,
	&kr_channel_map,
	&sg_channel_map,
	&ca_channel_map,
	&channel_map_terminate
};

#define CHANNEL_MAP_SIZE (sizeof(mapped_channel) / sizeof(*mapped_channel))

static enum chan_scheme morse_get_chan_scheme()
{
	FILE *f = fopen("/sys/module/dot11ah/parameters/channelization_scheme", "r");
	if (!f) {
		/* This indicates an old driver. */
		return CHAN_SCHEME_80211_2020;
	}

	int r = fgetc(f);
	fclose(f);
	switch (r) {
	case '1':
		return CHAN_SCHEME_80211_2020;
	case '2':
		return CHAN_SCHEME_80211_2024;
	case '3':
		return CHAN_SCHEME_80211_REVMF;
	default:
		/* A driver that has something we don't understand;
		 * default to what we think would be the driver default
		 * for now.
		 */
		return CHAN_SCHEME_80211_REVMF;
	};
}

void s1g_get_country(char *buf)
{
	FILE *f = fopen("/sys/module/morse/parameters/country", "r");
	if (!f) {
		buf[0] = '\0';
		return;
	}

	if (fread(buf, 1, 2, f) != 2) {
		buf[0] = '\0';
	}

	fclose(f);
}

country_channel_map_t *set_s1g_channel_map(void)
{
	char curr_country[3] = {0};
	enum chan_scheme curr_scheme = morse_get_chan_scheme();

	s1g_get_country(curr_country);

	for (int i = 0; i < CHANNEL_MAP_SIZE; i++) {
		const country_channel_map_t *map = mapped_channel[i];
		if (map->chan_schemes & curr_scheme && !strncmp(curr_country, map->country, strlen(map->country))) {
			return mapped_channel[i];
		}
	}

	return NULL;
}


channel_to_halow_freq_t *get_s1g(country_channel_map_t *map, int channel)
{
	if(map == NULL)
		return &kNullAhValue;

    for(int i=0;i<map->num_mapped_channels;i++)
    {
        if(map->ah_vals[i].channel==channel)
            return &map->ah_vals[i];
    }
    return &kNullAhValue;
}

float get_freq(country_channel_map_t *map, int channel)
{
	if(map == NULL)
		return 0;

	for(int i=0; i< map->num_mapped_channels; i++)
	{
		if(map->ah_vals[i].halow_channel==channel)
			return map->ah_vals[i].halow_freq;
	}

	return 0;

}

int s1g_rate(int fiveG_rate, int frq_mhz)
{
    int sc_map_5g[][2] = {
        {20 , 52},
        {40 , 108},
        {80 , 234},
        {160 , 468}};
    int sc_map_s1g[][2] = {
        {20 , 24},
        {40 , 52},
        {80 , 108},
        {160 , 234}};

    int index=-1;
    for (int i = 0; i < sizeof(sc_map_5g) / sizeof(int[2]); i++)
    {
        if(sc_map_5g[i][0] == frq_mhz)
        {
            index = i;
            break;
        }
    }
    int scale = 20; // for s1g we need to scale the reported shim layer values. if not exist approximate.
    if (index != -1)
    {
        scale = 10 * sc_map_5g[index][1] / sc_map_s1g[index][1];
    }
    return fiveG_rate / scale;
}

int s1g_chan2bw(country_channel_map_t *map,int channel)//bw in MHz
{
	if(map == NULL)
		return 0;

	for(int i=0; i< map->num_mapped_channels; i++)
	{
		if ((map->ah_vals[i].halow_channel) == channel)
			return map->ah_vals[i].bw;
	}

	return 0;
}

const country_channel_map_t** s1g_mapped_channel()
{
	return mapped_channel;
}

// returns true if this raw is the selected rate.
int mmrc_table_active_raw(const char* line)
{
    //check if it has MHz and MCS and GI keywords.
    if (strstr(line, "MCS") == NULL)
        return 0;
    if (strstr(line, "MHz") == NULL)
        return 0;
    if ((strstr(line, "SGI") == NULL) && (strstr(line, "LGI") == NULL))
        return 0;
    //start search from 17th character.
    if (strstr(line+17,"A"))
        return 1;
return 0;

}
//returns the avg tp from the selected line.
int get_mmrc_table_raw_throughput_avg(const char* line)
{
    float tp_avg;
	sscanf(line + 55, "%f", &tp_avg);
	return tp_avg*1000;
}

int get_mmrc_throughput(const char* phyname)
{
    FILE *file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char table_path[64];
    int rate_kbps=-1;

    sprintf (table_path,"/sys/kernel/debug/ieee80211/%s/morse/mmrc_table",phyname);
	file = fopen(table_path, "r");
    if (file == NULL)
    {
        return -1;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        if(mmrc_table_active_raw(line))
        {
            rate_kbps = get_mmrc_table_raw_throughput_avg(line);
			break;
        }
    }
	fclose(file);
    if (line)
        free(line);

	if(rate_kbps == 0)
		rate_kbps+=1; //to make sure that assoc list doesn't show "unknown" when there's no traffic.
    return rate_kbps;
}