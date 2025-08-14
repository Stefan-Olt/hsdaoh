/*
 * hsdaoh - High Speed Data Acquisition over MS213x USB3 HDMI capture sticks
 *
 * Copyright (C) 2024 by Steve Markgraf <steve@steve-m.de>
 *
 * based on librtlsdr:
 * Copyright (C) 2012-2024 by Steve Markgraf <steve@steve-m.de>
 * Copyright (C) 2012 by Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HSDAOH_RAW_H
#define __HSDAOH_RAW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hsdaoh_export.h>

#define HSDAOH_MAGIC	0xda7acab1

enum crc_config {
	CRC_NONE,			/* No CRC, just 16 bit idle counter */
	CRC16_1_LINE,		/* Line contains CRC of the last line */
	CRC16_2_LINE		/* Line contains CRC of the line before the last line */
};

#define DEFAULT_MAX_STREAMS		8

typedef struct
{
	uint64_t data_cnt;
	uint32_t srate;
	uint32_t reserved1;
	char reserved2[16];
} __attribute__((packed, aligned(1))) stream_info_t;

typedef struct
{
	uint32_t magic;
	uint16_t framecounter;
	uint8_t  reserved1;
	uint8_t  crc_config;
	uint16_t version;
	uint32_t flags;
	uint32_t reserved2[8];
	uint16_t stream0_format;
	uint16_t max_streamid;
	stream_info_t stream_info[DEFAULT_MAX_STREAMS];
} __attribute__((packed, aligned(1))) metadata_t;

#define FLAG_STREAM_ID_PRESENT	(1 << 0)
#define FLAG_FORMAT_ID_PRESENT	(1 << 1)

/* Extract the metadata stored in the upper 4 bits of the last word of each line */
static inline void hsdaoh_extract_metadata(uint8_t *data, metadata_t *metadata, unsigned int width)
{
	int j = 0;
	uint8_t *meta = (uint8_t *)metadata;

	for (unsigned i = 0; i < sizeof(metadata_t)*2; i += 2)
		meta[j++] = (data[((i+1)*width*2) - 1] >> 4) | (data[((i+2)*width*2) - 1] & 0xf0);
}

HSDAOH_API int hsdaoh_check_idle_cnt(hsdaoh_dev_t *dev, uint16_t *buf, size_t length);

HSDAOH_API void hsdaoh_raw_callback(hsdaoh_dev_t *dev, bool raw_cb);

#ifdef __cplusplus
}
#endif

#endif // __HSDAOH_RAW_H
