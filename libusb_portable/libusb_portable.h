/********************************** (C) COPYRIGHT *******************************
* File Name          : libusb_portable.h
* Author             : bvernoux
* Version            : V1.0.2
* Date               : 2022/08/31
* Description        :
* Copyright (c) 2022 Benjamin VERNOUX
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __LIBUSB_PORTABLE_H__
#define __LIBUSB_PORTABLE_H__

#include <sys/types.h>
#include "libusb.h"

#ifndef _WIN32
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define LIBUSB_BULK_TRANSFER_TIMEOUT_MILLISEC (2000)

#define USB3_EP1_BULK_BURST_SIZE (4*1024) // This size corresponds to USB3 EP1 Bulk with 4 burst(so 4x1024 bytes)
// Warning USB3 enpoint 2 bulk with 8 or 16 burst can be problematic on some PC so default is set to 4
#define USB3_EP2_BULK_BURST_SIZE (4*1024) // This size corresponds to USB3 EP2 Bulk with 4 burst(so 4x1024 bytes)
#define USB2_BULK_SIZE 512

uint32_t swap_uint32(uint32_t val);

float TimevalDiff(const struct timeval* a, const struct timeval* b);
void get_CurrentTime(char* date_time_ms, int date_time_ms_max_size);

void sleep_ms(int milliseconds); // Cross-platform sleep in milliseconds function
void sleep_us(int microseconds); // Cross-platform sleep in microseconds function

void print_hex(uint8_t* data, uint16_t size);

void log_printf_init(FILE *log_file_handle); /* Required before to call log_printf_dbg()/log_printf() API */

void log_printf_dbg(const char* fmt, ...);
void log_printf(const char* fmt, ...);

// return 0 if ok or error code
int usb_init(void);
/*
Return 0 if error or struct libusb_device_handle * if success
*/
struct libusb_device_handle* usb_opendev(int verbose);

void usb_closedev(struct libusb_device_handle* libusb_dev_handle);

void usb_exit(void);

/*
Return usb speed see enum libusb_speed
*/
enum libusb_speed usb_get_device_speed(libusb_device_handle *handle);

/* data_tx shall contains USB3_EP1_BULK_BURST_SIZE
   It is mandatory to send always USB3_EP1_BULK_BURST_SIZE (unused data are ignored)
   return 1 if ok or error code
*/
int usb_write_EP1(libusb_device_handle *handle, uint8_t* data_tx);

/* data_rx will contains USB3_EP1_BULK_BURST_SIZE bytes
   It is mandatory to receive always USB3_EP1_BULK_BURST_SIZE bytes (unused data are ignored)
   return 1 if ok or error code
*/
int usb_read_EP1(libusb_device_handle *handle, uint8_t* data_rx);

/*
   return 1 if ok or error code
*/
int usb_write_EP2(libusb_device_handle *handle, uint32_t* data_tx, int data_size);

/*
   return 1 if ok or error code
*/
int usb_read_EP2(libusb_device_handle *handle, uint32_t* data_rx, int data_size);

/* Return 0 if success other value are error code */
int USB_TestDataIntegrity(struct libusb_device_handle *handle, uint32_t *writebuf, uint32_t *readbuf, int test_num);

/* Return 0 if success other value are error code */
int USB_TestDataSpeed(struct libusb_device_handle *handle, uint32_t *writebuf, uint32_t *readbuf, int test_num, int test_data_len);


#ifdef __cplusplus
}
#endif

#endif  /* __LIBUSB_PORTABLE_H__ */