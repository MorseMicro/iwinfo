/*
 * iwinfo - Wireless Information Library - Utility Headers
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
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

#ifndef __IWINFO_UTILS_H_
#define __IWINFO_UTILS_H_

#include <sys/socket.h>
#include <net/if.h>
#include <uci.h>
#include <libubus.h>

#include "iwinfo.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#define LOG10_MAGIC	1.25892541179

extern struct uci_context *uci_ctx;

int iwinfo_ioctl(int cmd, void *ifr);

int iwinfo_dbm2mw(int in);
int iwinfo_mw2dbm(int in);
static inline int iwinfo_mbm2dbm(int gain)
{
	return gain / 100;
}

const char * const iwinfo_band_name(int mask);
const char * const iwinfo_htmode_name(int mask);

uint32_t iwinfo_band2ghz(uint8_t band);
uint8_t iwinfo_ghz2band(uint32_t ghz);

size_t iwinfo_format_hwmodes(int modes, char *buf, size_t len);
int iwinfo_htmode_is_ht(int htmode);
int iwinfo_htmode_is_vht(int htmode);
int iwinfo_htmode_is_he(int htmode);
int iwinfo_htmode_is_eht(int htmode);

int iwinfo_ifup(const char *ifname);
int iwinfo_ifdown(const char *ifname);
int iwinfo_ifmac(const char *ifname);

void iwinfo_close(void);

struct iwinfo_hardware_entry * iwinfo_hardware(struct iwinfo_hardware_id *id);

int iwinfo_hardware_id_from_mtd(struct iwinfo_hardware_id *id);

void iwinfo_parse_rsn(struct iwinfo_crypto_entry *c, uint8_t *data, uint8_t len,
					  uint16_t defcipher, uint8_t defauth);
void iwinfo_parse_rsnxe(struct iwinfo_crypto_entry *c, uint8_t *data, uint8_t len);

struct uci_section *iwinfo_uci_get_radio(const char *name, const char *type);
void iwinfo_uci_free(void);

int iwinfo_ubus_query(const char *ifname, const char *field,
                      char *buf, size_t len);

/*
For a 1 MHz Wi-Fi HaLow channel, the theoretical RSSI range can be from -120 dBm to +30 dBm. When two APs are
placed next to each other, the chip may report a positive RSSI value. Based on discussions with the hardware team,
the chip may report icorrect RSSI when the signal is too strong.
However, the aim is to pass the recieved RSSI as is to the upper layer.

The 'iwinfo' utility assumes RSSI will always be negative and maps (0) to (-256). It interprets positive
RSSI values incorrectly and wraps them into a negative numbers. For example, a signal strength reported
as 1 dBm to 30 dBm is incorrectly shown as -255 dBm to -226 dBm. This is because 'iwinfo' stores the signal
 value as uint8_t, but while printing, it casts it to a signed value, leading to incorrect negative numbers.
This API makes room for positive RSSI by converting into signed interger
(128) to (256) maps to (-128) - (  0)
(  0) to (127) maps to (   0) - (127)
*/
static inline int iwinfo_sanitise_rssi(uint8_t signal) {
	if (signal < 128) return signal;
	return (signal - 0x100);
}

#endif
