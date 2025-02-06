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
	.num_mapped_channels = ARRAY_SIZE(us_s1g_channels),
	.ah_vals = us_s1g_channels,
};

/* AU map */
static const country_channel_map_t au_channel_map = {
	.country = "AU",
	.num_mapped_channels = ARRAY_SIZE(au_s1g_channels),
	.ah_vals = au_s1g_channels,
};

/* NZ map */
static const country_channel_map_t nz_channel_map = {
	.country = "NZ",
	.num_mapped_channels = ARRAY_SIZE(nz_s1g_channels),
	.ah_vals = nz_s1g_channels,
};

/* EU map */
static const country_channel_map_t eu_channel_map = {
	.country = "EU",
	.num_mapped_channels = ARRAY_SIZE(eu_s1g_channels),
	.ah_vals = eu_s1g_channels,
};

/* IN map */
static const country_channel_map_t in_channel_map = {
	.country = "IN",
	.num_mapped_channels = ARRAY_SIZE(in_s1g_channels),
	.ah_vals = in_s1g_channels,
};

/* JP map */
static const country_channel_map_t jp_channel_map = {
	.country = "JP",
	.num_mapped_channels = ARRAY_SIZE(jp_s1g_channels),
	.ah_vals = jp_s1g_channels,
};

/* KR map */
static const country_channel_map_t kr_channel_map = {
	.country = "KR",
	.num_mapped_channels = ARRAY_SIZE(kr_s1g_channels),
	.ah_vals = kr_s1g_channels,
};

/* SG map */
static const country_channel_map_t sg_channel_map = {
	.country = "SG",
	.num_mapped_channels = ARRAY_SIZE(sg_s1g_channels),
	.ah_vals = sg_s1g_channels,
};

static const country_channel_map_t ca_channel_map = {
	.country = "CA",
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
	&au_channel_map,
	&nz_channel_map,
	&eu_channel_map,
	&in_channel_map,
	&jp_channel_map,
	&kr_channel_map,
	&sg_channel_map,
	&ca_channel_map,
	&channel_map_terminate
};

#define CHANNEL_MAP_SIZE (sizeof(mapped_channel) / sizeof(*mapped_channel))

static void morse_get_country(country_channel_map_t *halow_vals)
{
	FILE *country_parameter;

	country_parameter = fopen("/sys/module/morse/parameters/country", "r");
	fscanf(country_parameter, "%2s", halow_vals->country);
	fclose(country_parameter);
}

country_channel_map_t *set_s1g_channel_map(void)
{
	country_channel_map_t halow_vals;

	morse_get_country(&halow_vals);
	if (strlen(halow_vals.country) != 0)
	{
		for (int i = 0; i < CHANNEL_MAP_SIZE; i++)
		{
			if (!strncmp(halow_vals.country, mapped_channel[i]->country, strlen(mapped_channel[i]->country)))
			{
				return mapped_channel[i];
			}
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

int s1g_freq2channel(country_channel_map_t *map,int freq)//frq in khz
{
	if(map == NULL)
		return 0;

	for(int i=0; i< map->num_mapped_channels; i++)
	{
		if ((int)(map->ah_vals[i].halow_freq * 1000) == freq)
			return map->ah_vals[i].halow_channel;
	}

	return 0;
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

void s1g_get_country(char *buf)
{
	country_channel_map_t halow_vals;
	morse_get_country(&halow_vals);
	memcpy(buf,halow_vals.country,2);
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