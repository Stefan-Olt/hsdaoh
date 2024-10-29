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

#ifndef __HSDAOH_H
#define __HSDAOH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <hsdaoh_export.h>

typedef struct hsdaoh_dev hsdaoh_dev_t;

/** HSDAOH message types
 */
typedef enum hsdaoh_message_types {
  HSDAOH_MSG_INFO = 0,
  HSDAOH_MSG_WARNING = 1,
  HSDAOH_MSG_ERROR = 2
} hsdaoh_message_types_t;

/** HSDAOH messages
 */
typedef enum hsdaoh_messages {
  HSDAOH_SUCCESS = 0,
  /** errors **/
  HSDAOH_ERROR_KERNEL_UVC_DRIVER_DETACH_FAILED = -1,
  HSDAOH_ERROR_KERNEL_HID_DRIVER_DETACH_FAILED = -2,
  HSDAOH_ERROR_KERNEL_DRIVER_REATTACH_FAILED = -3,
  HSDAOH_ERROR_USB_CLAIM_INTERFACE_HID_FAILED = -4,
  HSDAOH_ERROR_USB_CLAIM_INTERFACE_1_FAILED = -5,
  HSDAOH_ERROR_USB_CLEARING_ENDPOINT_HALT_FAILED = -6,
  HSDAOH_ERROR_USB_OPEN_FAILED = -7,
  HSDAOH_ERROR_USB_ACCESS = -8,
  HSDAOH_ERROR_INCORRECT_FRAME_FORMAT = -9,
  HSDAOH_ERROR_OTHER = -99,
  /** warnings **/
  HSDAOH_WARNING_MISSED_FRAME = 1,
  HSDAOH_WARNING_INVALID_PAYLOAD_LENGTH = 2,
  HSDAOH_WARNING_IDLE_COUNTER_ERROR = 3,
  /** infos **/
  HSDAOH_INFO_KERNEL_REATTACH_DRIVER = 100,
  HSDAOH_INFO_SYNCRONIZED_HDMI_INPUT_STREAM = 101,
  HSDAOH_INFO_START_STREAMING = 102,
  HSDAOH_INFO_STOP_STREAMING = 103
} hsdaoh_messages_t;

typedef void(*hsdaoh_message_cb_t)(int msg_type, int msg, void *additional, void *ctx);

/*!
 * Get string from message code.
 *
 * NOTE: The string argument must provide space for up to 256 bytes.
 *
 * \param msg_type the type of the message
 * \param mag the message code
 * \param additional data associated with the message
 * \param output string, may be NULL (then output to stderr)
 * \return 0 on success
 */
HSDAOH_API int hsdaoh_get_message_string(int msg_type,
					     int msg,
					     void *additional,
					     char *output);

HSDAOH_API uint32_t hsdaoh_get_device_count(void);

HSDAOH_API const char* hsdaoh_get_device_name(uint32_t index);

/*!
 * Get USB device strings.
 *
 * NOTE: The string arguments must provide space for up to 256 bytes.
 *
 * \param index the device index
 * \param manufact manufacturer name, may be NULL
 * \param product product name, may be NULL
 * \param serial serial number, may be NULL
 * \return 0 on success
 */
HSDAOH_API int hsdaoh_get_device_usb_strings(uint32_t index,
					     char *manufact,
					     char *product,
					     char *serial);

/*!
 * Get USB strings.
 *
 * NOTE: The string arguments must provide space for up to 256 bytes.
 *
 * \param dev the device handle given by hsdaoh_open()
 * \param manufact manufacturer name, may be NULL
 * \param product product name, may be NULL
 * \param serial serial number, may be NULL
 * \return 0 on success
 */

HSDAOH_API int hsdaoh_get_usb_strings(hsdaoh_dev_t *dev,
					     char *manufact,
					     char *product,
					     char *serial);

/*!
 * Get device index by USB serial string descriptor.
 *
 * \param serial serial string of the device
 * \return device index of first device where the name matched
 * \return -1 if name is NULL
 * \return -2 if no devices were found at all
 * \return -3 if devices were found, but none with matching name
 */
HSDAOH_API int hsdaoh_get_index_by_serial(const char *serial);

HSDAOH_API int hsdaoh_open(hsdaoh_dev_t **dev, uint32_t index);

HSDAOH_API int hsdaoh_open_msg_cb(hsdaoh_dev_t **dev, uint32_t index, hsdaoh_message_cb_t cb, void *ctx);

HSDAOH_API int hsdaoh_close(hsdaoh_dev_t *dev);

/* configuration functions */

/*!
 * Get USB device strings.
 *
 * NOTE: The string arguments must provide space for up to 256 bytes.
 *
 * \param dev the device handle given by hsdaoh_open()
 * \param manufact manufacturer name, may be NULL
 * \param product product name, may be NULL
 * \param serial serial number, may be NULL
 * \return 0 on success
 */
HSDAOH_API int hsdaoh_get_usb_strings(hsdaoh_dev_t *dev, char *manufact,
				      char *product, char *serial);

/*!
 * Set the sample rate for the device
 *
 * \param dev the device handle given by hsdaoh_open()
 * \param samp_rate the sample rate to be set
 * \param ext_clock if true, use the IFCLK input insteafd of internal clock source
 *		    if a Si5351 is connected, it will be configured
 * \return 0 on success, -EINVAL on invalid rate
 */
HSDAOH_API int hsdaoh_set_sample_rate(hsdaoh_dev_t *dev, uint32_t rate, bool ext_clock);

/*!
 * Get actual sample rate the device is configured to.
 *
 * \param dev the device handle given by hsdaoh_open()
 * \return 0 on error, sample rate in Hz otherwise
 */
HSDAOH_API uint32_t hsdaoh_get_sample_rate(hsdaoh_dev_t *dev);

/* streaming functions */

typedef void(*hsdaoh_read_cb_t)(unsigned char *buf, uint32_t len, uint8_t pack_state, void *ctx);

/*!
 * Start streaming data from the device.
 *
 * \param dev the device handle given by hsdaoh_open()
 * \param cb callback function to return received data
 * \param ctx user specific context to pass via the callback function
 * \return 0 on success
 */
HSDAOH_API int hsdaoh_start_stream(hsdaoh_dev_t *dev,
				   hsdaoh_read_cb_t cb,
				   void *ctx);

/*!
 * Stop streaming data from the device.
 *
 * \param dev the device handle given by hsdaoh_open()
 * \return 0 on success
 */
HSDAOH_API int hsdaoh_stop_stream(hsdaoh_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __HSDAOH_H */
