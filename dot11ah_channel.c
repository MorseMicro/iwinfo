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

static const channel_to_halow_freq_t kNullAhValue = {0, 0, 0, 0};

static const country_channel_map_t us_channel_map = {
	.country = "US",
	.num_mapped_channels = 48,
	.ah_vals = {
		/* 1Mhz */
		{132, 1, 902.5, 0},
		{136, 3, 903.5, 0},
		{36, 5, 904.5, 0},
		{40, 7, 905.5, 0},
		{44, 9, 906.5, 0},
		{48, 11, 907.5, 0},
		{52, 13, 908.5, 0},
		{56, 15, 909.5, 0},
		{60, 17, 910.5, 0},
		{64, 19, 911.5, 0},
		{100, 21, 912.5, 0},
		{104, 23, 913.5, 0},
		{108, 25, 914.5, 0},
		{112, 27, 915.5, 0},
		{116, 29, 916.5, 0},
		{120, 31, 917.5, 0},
		{124, 33, 918.5, 0},
		{128, 35, 919.5, 0},
		{149, 37, 920.5, 0},
		{153, 39, 921.5, 0},
		{157, 41, 922.5, 0},
		{161, 43, 923.5, 0},
		{165, 45, 924.5, 0},
		{169, 47, 925.5, 0},
		{173, 49, 926.5, 0},
		{177, 51, 927.5, 0},
		/* 2MHz */
		{134, 2, 903, 1},
		{38, 6, 905, 1},
		{46, 10, 907, 1},
		{54, 14, 909, 1},
		{62, 18, 911, 1},
		{102, 22, 913, 1},
		{110, 26, 915, 1},
		{118, 30, 917, 1},
		{126, 34, 919, 1},
		{151, 38, 921, 1},
		{159, 42, 923, 1},
		{167, 46, 925, 1},
		{175, 50, 927, 1},
		/* 4MHz */
		{42, 8, 906, 2},
		{58, 16, 910, 2},
		{106, 24, 914, 2},
		{122, 32, 918, 2},
		{155, 40, 922, 2},
		{171, 48, 926, 2},
		/* 8MHz */
		{50, 12, 908, 3},
		{114, 28, 916, 3},
		{163, 44, 924, 3},
	}
};

static const country_channel_map_t au_channel_map = {
	.country = "AU",
	.num_mapped_channels = 23,
	.ah_vals = {
		/* 1Mhz */
		{112, 27, 915.5, 0},
		{116, 29, 916.5, 0},
		{120, 31, 917.5, 0},
		{124, 33, 918.5, 0},
		{128, 35, 919.5, 0},
		{149, 37, 920.5, 0},
		{153, 39, 921.5, 0},
		{157, 41, 922.5, 0},
		{161, 43, 923.5, 0},
		{165, 45, 924.5, 0},
		{169, 47, 925.5, 0},
		{173, 49, 926.5, 0},
		{177, 51, 927.5, 0},
		/* 2Mhz */
		{118, 30, 917, 1},
		{126, 34, 919, 1},
		{151, 38, 921, 1},
		{159, 42, 923, 1},
		{167, 46, 925, 1},
		{175, 50, 927, 1},
		/* 4 Mhz */
		{122, 32, 918, 2},
		{155, 40, 922, 2},
		{171, 48, 926, 2},
		/* 8 Mhz */
		{163, 44, 924, 3}
	}
};

static const country_channel_map_t nz_channel_map = {
	.country = "NZ",
	.num_mapped_channels = 23,
	.ah_vals = {
		/* 1Mhz */
		{112, 27, 915.5, 0},
		{116, 29, 916.5, 0},
		{120, 31, 917.5, 0},
		{124, 33, 918.5, 0},
		{128, 35, 919.5, 0},
		{149, 37, 920.5, 0},
		{153, 39, 921.5, 0},
		{157, 41, 922.5, 0},
		{161, 43, 923.5, 0},
		{165, 45, 924.5, 0},
		{169, 47, 925.5, 0},
		{173, 49, 926.5, 0},
		{177, 51, 927.5, 0},
		/* 2Mhz */
		{118, 30, 917, 1},
		{126, 34, 919, 1},
		{151, 38, 921, 1},
		{159, 42, 923, 1},
		{167, 46, 925, 1},
		{175, 50, 927, 1},
		/* 4 Mhz */
		{122, 32, 918, 2},
		{155, 40, 922, 2},
		{171, 48, 926, 2},
		/* 8 Mhz */
		{163, 44, 924, 3}
	}
};

static const country_channel_map_t eu_channel_map = {
	.country = "EU",
	.num_mapped_channels = 8,
	.ah_vals = {
		/* 1Mhz */
		{132, 1, 863.5, 0},
		{136, 3, 864.5, 0},
		{36, 5, 865.5, 0},
		{40, 7, 866.5, 0},
		{44, 9, 867.5, 0},
		{120, 31, 916.9, 0},
		{124, 33, 917.9, 0},
		{128, 35, 918.9, 0},
	}
};

static const country_channel_map_t in_channel_map = {
	.country = "IN",
	.num_mapped_channels = 3,
	.ah_vals = {
		/* 1Mhz */
		{36, 5, 865.5, 0},
		{40, 7, 866.5, 0},
		{44, 9, 867.5, 0},
	}
};

static const country_channel_map_t jp_channel_map = {
	.country = "JP",
	.num_mapped_channels = 11,
	.ah_vals = {
		/* 1 Mhz */
		{36, 13, 923, 0},
		{40, 15, 924, 0},
		{44, 17, 925, 0},
		{48, 19, 926, 0},
		{64, 21, 927, 0},
		/* 2Mhz */
		{38, 2, 923.5, 1},
		{46, 6, 925.5, 1},
		{54, 4, 924.5, 1},
		{62, 8, 926.5, 1},
		/* 4Mhz */
		{42, 36, 924.5, 2},
		{58, 38, 925.5, 2},
	}
};

static const country_channel_map_t kr_channel_map = {
	.country = "KR",
	.num_mapped_channels = 10,
	.ah_vals = {
		/* 1 Mhz */
		{132, 1, 918, 0},
		{136, 3, 919, 0},
		{36, 5, 920, 0},
		{40, 7, 921, 0},
		{44, 9, 922, 0},
		{48, 11, 923, 0},
		/* 2Mhz */
		{134, 2, 918.5, 1},
		{38, 6, 920.5, 1},
		{46, 10, 922.5, 1},
		/* 4Mhz */
		{42, 8, 921.5, 2},
	}
};

static const country_channel_map_t sg_channel_map = {
	.country = "SG",
	.num_mapped_channels = 12,
	.ah_vals = {
		/* 1 Mhz */
		{40, 7, 866.5, 0},
		{44, 9, 867.5, 0},
		{48, 11, 868.5, 0},
		{149, 37, 920.5, 0},
		{153, 39, 921.5, 0},
		{157, 41, 922.5, 0},
		{161, 43, 923.5, 0},
		{165, 45, 924.5, 0},
		/* 2Mhz */
		{46, 10, 868, 1},
		{151, 38, 921, 1},
		{159, 42, 923, 1},
		/* 4Mhz */
		{155, 40, 922, 2}
	}
};

static const country_channel_map_t channel_map_terminate = {
	.country = {0,0,0},
	.num_mapped_channels = 0,
	.ah_vals = {}
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