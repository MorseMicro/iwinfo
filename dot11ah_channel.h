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

#ifndef __DOT11AH_CHANNELS__
#define __DOT11AH_CHANNELS__

typedef struct {
	/*5G channel*/
	int channel;
	/*80211ah channel*/
	int halow_channel;
	/*80211ah freq*/
	float halow_freq;
	/*80211ah bandwidth*/
	int bw;
} channel_to_halow_freq_t;

typedef struct {
	char country[3];
	int num_mapped_channels;
	channel_to_halow_freq_t ah_vals[];
} country_channel_map_t;


/**
 * @brief gets the channel 5g to s1g channel map for a given region.
 *        country setting is read from /sys/module/morse/parameters/country
 */
country_channel_map_t *set_s1g_channel_map(void);


/**
 * retrieves the map entry for a given 5g channel
 * @param map a map of 5g to s1g channels retrieved by set_s1g_channel_map
 * @param channel a 5g channel value
 * @return The entry in the map corresponding to the given 5g channel and configured region
 */
channel_to_halow_freq_t *get_s1g(country_channel_map_t *map, int channel);

/**
 * retrieves the s1g frequency for a given s1g channel
 * @param map a map of 5g to s1g channels retrieved by set_s1g_channel_map
 * @param channel an s1g channel value
 * @return a floating point value corresponding to the s1g frequency, in mhz, for the given channel.
 */
float get_freq(country_channel_map_t *map, int channel);

/**
 * a conversion helper for calculating the correct s1g mcs data rate for a given bandwidth and existing 5g mcs rate
 * @param fiveG_rate a 5g mcs data rate value
 * @param frq_mhz the 5g bandwidth corresponding to the 5g rate
 * @return the s1g mcs data rate
 */
int s1g_rate(int fiveG_rate, int frq_mhz);

/**
 * converts s1g frequency to s1g channel
 * @param map a map of 5g to s1g channels retrieved by set_s1g_channel_map
 * @param freq s1g frequency in KHz
 * @return s1g channel
 */
int s1g_freq2channel(country_channel_map_t *map,int freq);

/**
 * converts s1g channel to s1g bandwidth
 * @param map a map of 5g to s1g channels retrieved by set_s1g_channel_map
 * @param channel s1g channel
 * @return s1g channel
 */
int s1g_chan2bw(country_channel_map_t *map,int channel);

/**
 * gives a pointer to the country separated channels map
 * @return a pointer to the country separated channels map. 
 */
const country_channel_map_t** s1g_mapped_channel();


/**
 * returns the currently set country. 
 * @param buf a pointer to the result buffer.
 */
void s1g_get_country(const char *buf);

/**
 * gets mmrc average throughput from the mmrc_table.
 * @param phyname phyname of the halow device. 
 * @return average throughput in kbps 
 */
int get_mmrc_throughput(const char* phyname);

#endif /* __DOT11AH_CHANNELS__ */
