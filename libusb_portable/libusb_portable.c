/********************************** (C) COPYRIGHT *******************************
* File Name          : libusb_portable.c
* Author             : bvernoux
* Version            : V1.0.2
* Date               : 2022/08/31
* Description        :
* Copyright (c) 2022 Benjamin VERNOUX
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#ifndef _WIN32
#include <unistd.h> // usleep ...
#endif

#ifdef _WIN32
#include <windows.h> // QueryPerformanceCounter ... 
#endif

#include "libusb_portable.h"
#include "hydrausb3_usb_devbulk_vid_pid.h"

#define __USE_MINGW_ANSI_STDIO 1 // Required for MSYS2 mingw64 to support format "%z" ...

/* HydraUSB3 v1 VID PID */
#define VID USB_VID // USB vendor id
#define PID USB_PID // USB product id

/*
IN and OUT transactions/endpoints is from the perspective of the host
IN data travels from the device to the host.
OUT data travels from the host to the device.
*/
/* EP1 used to execute some commands/retrieve specific data */
#define BULK_EP1_IN  (LIBUSB_ENDPOINT_IN  | 1) // EP1 In: device-to-host
#define BULK_EP1_OUT (LIBUSB_ENDPOINT_OUT | 1) // EP1 Out: host-to-device

/* EP2 used for USB2/USB3 data streaming */
#define BULK_EP2_IN  (LIBUSB_ENDPOINT_IN  | 2) // EP2 In: device-to-host
#define BULK_EP2_OUT (LIBUSB_ENDPOINT_OUT | 2) // EP2 Out: host-to-device

#define CURR_TIME_SIZE (40)
static char currTime[CURR_TIME_SIZE + 1] = "";
static struct timeval start_tv;
static struct timeval curr_tv;

static FILE *pFile = NULL; /* File for logging */

static enum libusb_speed usb_speed;

static struct libusb_device_handle *handle = NULL;
int usb_claim_interface = 0;

void log_printf_init(FILE *log_file_handle)
{
	pFile = log_file_handle;
	gettimeofday(&start_tv, NULL); // Global time
}

//! Byte swap unsigned int
uint32_t swap_uint32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

float TimevalDiff(const struct timeval* a, const struct timeval* b)
{
	return (a->tv_sec - b->tv_sec) + 1e-6f * (a->tv_usec - b->tv_usec);
}

#ifdef _WIN32
int gettimeofday(struct timeval* tv, void* ignored)
{
	FILETIME ft;
	unsigned __int64 tmp = 0;
	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);
		tmp |= ft.dwHighDateTime;
		tmp <<= 32;
		tmp |= ft.dwLowDateTime;
		tmp /= 10;
		tmp -= 11644473600000000ULL;
		tv->tv_sec = (long)(tmp / 1000000UL);
		tv->tv_usec = (long)(tmp % 1000000UL);
	}
	return 0;
}
#endif

void get_CurrentTime(char* date_time_ms, int date_time_ms_max_size)
{
#define CURRENT_TIME_SIZE (30)
	char currentTime[CURRENT_TIME_SIZE + 1] = "";
	time_t rawtime;
	struct tm* timeinfo;

	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(currentTime, CURRENT_TIME_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);
	snprintf(date_time_ms, (date_time_ms_max_size - 1), "%s.%03d", currentTime, milli);
}

void log_printf_dbg(const char* fmt, ...)
{
#define TIME_STR_LEN (100)
	char time_str[TIME_STR_LEN+1];
	va_list args;

	gettimeofday(&curr_tv, NULL);
	get_CurrentTime(currTime, CURR_TIME_SIZE);

	snprintf(time_str, TIME_STR_LEN, "%s (%05.03f s) ", currTime, TimevalDiff(&curr_tv, &start_tv));
	fputs(time_str, stdout);
	if(pFile != NULL)
	{
		fputs(time_str, pFile);
	}

	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fflush(stdout);

	if(pFile != NULL)
	{
		va_start(args, fmt);
		vfprintf(pFile, fmt, args);
		va_end(args);
		fflush(pFile);
	}
}

void log_printf(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
	fflush(stdout);

	if(pFile != NULL)
	{
		va_start(args, fmt);
		vfprintf(pFile, fmt, args);
		va_end(args);
		fflush(pFile);
	}

}

void sleep_ms(int milliseconds) // Cross-platform sleep in milliseconds function
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif // win32
}

void sleep_us(int microseconds) // Cross-platform sleep in microseconds function
{
#ifdef _WIN32
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *) &time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	do
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time2);
	}
	while((time2-time1) < microseconds);
#else
	usleep(microseconds);
#endif // win32
}

void print_hex(uint8_t* data, uint16_t size)
{
	uint16_t i, j;
	for (i = 0; i < size; ++i)
	{
		log_printf("%02X ", data[i]);
		if ((i+1) % 8 == 0 || i+1 == size)
		{
			log_printf(" ");
			if ((i+1) % 16 == 0)
			{
				log_printf("\n");
			}
			else if (i+1 == size)
			{
				if ((i+1) % 16 <= 8)
				{
					log_printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j)
				{
					log_printf("   ");
				}
				log_printf("\n");
			}
		}
	}
}

static void print_endpoint_comp(const struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
	log_printf("        USB 3.0 Endpoint Companion:\n");
	log_printf("          bMaxBurst:           %u\n", ep_comp->bMaxBurst);
	log_printf("          bmAttributes:        %02xh\n", ep_comp->bmAttributes);
	log_printf("          wBytesPerInterval:   %u\n", ep_comp->wBytesPerInterval);
}

static void print_endpoint(const struct libusb_endpoint_descriptor *endpoint)
{
	int i, ret;
	char* endpoint_in_out;
	if((endpoint->bEndpointAddress&0x80) > 0)
		endpoint_in_out = "IN";
	else
		endpoint_in_out = "OUT";
	log_printf("      Endpoint%d %s:\n", (endpoint->bEndpointAddress&0x7F), endpoint_in_out);
	log_printf("        bEndpointAddress:    %02xh\n", endpoint->bEndpointAddress);
	log_printf("        bmAttributes:        %02xh\n", endpoint->bmAttributes);
	log_printf("        wMaxPacketSize:      %u\n", endpoint->wMaxPacketSize);
	log_printf("        bInterval:           %u\n", endpoint->bInterval);
	log_printf("        bRefresh:            %u\n", endpoint->bRefresh);
	log_printf("        bSynchAddress:       %u\n", endpoint->bSynchAddress);

	for (i = 0; i < endpoint->extra_length;)
	{
		if (LIBUSB_DT_SS_ENDPOINT_COMPANION == endpoint->extra[i + 1])
		{
			struct libusb_ss_endpoint_companion_descriptor *ep_comp;

			ret = libusb_get_ss_endpoint_companion_descriptor(NULL, endpoint, &ep_comp);
			if (LIBUSB_SUCCESS != ret)
				continue;

			print_endpoint_comp(ep_comp);

			libusb_free_ss_endpoint_companion_descriptor(ep_comp);
		}

		i += endpoint->extra[i];
	}
}

static void print_altsetting(const struct libusb_interface_descriptor *interface)
{
	uint8_t i;

	log_printf("    Interface:\n");
	log_printf("      bInterfaceNumber:      %u\n", interface->bInterfaceNumber);
	log_printf("      bAlternateSetting:     %u\n", interface->bAlternateSetting);
	log_printf("      bNumEndpoints:         %u\n", interface->bNumEndpoints);
	log_printf("      bInterfaceClass:       %u\n", interface->bInterfaceClass);
	log_printf("      bInterfaceSubClass:    %u\n", interface->bInterfaceSubClass);
	log_printf("      bInterfaceProtocol:    %u\n", interface->bInterfaceProtocol);
	log_printf("      iInterface:            %u\n", interface->iInterface);

	for (i = 0; i < interface->bNumEndpoints; i++)
	{
		print_endpoint(&interface->endpoint[i]);
	}
}

static void print_2_0_ext_cap(struct libusb_usb_2_0_extension_descriptor *usb_2_0_ext_cap)
{
	log_printf("    USB 2.0 Extension Capabilities:\n");
	log_printf("      bDevCapabilityType:    %u\n", usb_2_0_ext_cap->bDevCapabilityType);
	log_printf("      bmAttributes:          %08xh\n", usb_2_0_ext_cap->bmAttributes);
}

static void print_ss_usb_cap(struct libusb_ss_usb_device_capability_descriptor *ss_usb_cap)
{
	log_printf("    USB 3.0 Capabilities:\n");
	log_printf("      bDevCapabilityType:    %u\n", ss_usb_cap->bDevCapabilityType);
	log_printf("      bmAttributes:          %02xh\n", ss_usb_cap->bmAttributes);
	log_printf("      wSpeedSupported:       %u\n", ss_usb_cap->wSpeedSupported);
	log_printf("      bFunctionalitySupport: %u\n", ss_usb_cap->bFunctionalitySupport);
	log_printf("      bU1devExitLat:         %u\n", ss_usb_cap->bU1DevExitLat);
	log_printf("      bU2devExitLat:         %u\n", ss_usb_cap->bU2DevExitLat);
}

static void print_bos(libusb_device_handle *handle)
{
	struct libusb_bos_descriptor *bos;
	uint8_t i;
	int ret;

	ret = libusb_get_bos_descriptor(handle, &bos);
	if (ret < 0)
		return;

	log_printf("  Binary Object Store (BOS):\n");
	log_printf("    wTotalLength:            %u\n", bos->wTotalLength);
	log_printf("    bNumDeviceCaps:          %u\n", bos->bNumDeviceCaps);

	for (i = 0; i < bos->bNumDeviceCaps; i++)
	{
		struct libusb_bos_dev_capability_descriptor *dev_cap = bos->dev_capability[i];

		if (dev_cap->bDevCapabilityType == LIBUSB_BT_USB_2_0_EXTENSION)
		{
			struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension;

			ret = libusb_get_usb_2_0_extension_descriptor(NULL, dev_cap, &usb_2_0_extension);
			if (ret < 0)
				return;

			print_2_0_ext_cap(usb_2_0_extension);
			libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
		}
		else if (dev_cap->bDevCapabilityType == LIBUSB_BT_SS_USB_DEVICE_CAPABILITY)
		{
			struct libusb_ss_usb_device_capability_descriptor *ss_dev_cap;

			ret = libusb_get_ss_usb_device_capability_descriptor(NULL, dev_cap, &ss_dev_cap);
			if (ret < 0)
				return;

			print_ss_usb_cap(ss_dev_cap);
			libusb_free_ss_usb_device_capability_descriptor(ss_dev_cap);
		}
	}

	libusb_free_bos_descriptor(bos);
}

static void print_interface(const struct libusb_interface *interface)
{
	int i;

	for (i = 0; i < interface->num_altsetting; i++)
		print_altsetting(&interface->altsetting[i]);
}

static void print_configuration(struct libusb_config_descriptor *config)
{
	uint8_t i;

	log_printf("  Configuration:\n");
	log_printf("    wTotalLength:            %u\n", config->wTotalLength);
	log_printf("    bNumInterfaces:          %u\n", config->bNumInterfaces);
	log_printf("    bConfigurationValue:     %u\n", config->bConfigurationValue);
	log_printf("    iConfiguration:          %u\n", config->iConfiguration);
	log_printf("    bmAttributes:            %02xh\n", config->bmAttributes);
	log_printf("    MaxPower:                %u\n", config->MaxPower);

	for (i = 0; i < config->bNumInterfaces; i++)
		print_interface(&config->interface[i]);
}

static void print_device(libusb_device *dev, libusb_device_handle *handle, int verbose)
{
	struct libusb_device_descriptor desc;
	const char *speed;
	int ret;

	usb_speed = libusb_get_device_speed(dev);
	switch (usb_speed)
	{
		case LIBUSB_SPEED_LOW:
			speed = "1.5M";
			break;
		case LIBUSB_SPEED_FULL:
			speed = "12M";
			break;
		case LIBUSB_SPEED_HIGH:
			speed = "480M";
			break;
		case LIBUSB_SPEED_SUPER:
			speed = "5G";
			break;
		case LIBUSB_SPEED_SUPER_PLUS:
			speed = "10G";
			break;
		default:
			speed = "Unknown";
	}

	ret = libusb_get_device_descriptor(dev, &desc);
	if (ret < 0)
	{
		fprintf(stderr, "failed to get device descriptor");
		return;
	}

	log_printf_dbg("Dev (bus %u, device %u): %04X - %04X speed: %s\n",
				   libusb_get_bus_number(dev), libusb_get_device_address(dev),
				   desc.idVendor, desc.idProduct, speed);

	if (verbose)
	{
		if (handle)
		{
			unsigned char string[256];
			if (desc.iManufacturer)
			{
				ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, string, sizeof(string));
				if (ret > 0)
					log_printf("  Manufacturer: %s\n", (char *)string);
			}
			if (desc.iProduct)
			{
				ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, string, sizeof(string));
				if (ret > 0)
					log_printf("  Product: %s\n", (char *)string);
			}
			if (desc.iSerialNumber)
			{
				ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, string, sizeof(string));
				if (ret > 0)
					log_printf("  Serial Number: %s\n", (char *)string);
			}
		}
		for (int i = 0; i < desc.bNumConfigurations; i++)
		{
			struct libusb_config_descriptor *config;

			ret = libusb_get_config_descriptor(dev, i, &config);
			if (LIBUSB_SUCCESS != ret)
			{
				log_printf("  Couldn't retrieve descriptors\n");
				continue;
			}

			print_configuration(config);

			libusb_free_config_descriptor(config);
		}

		if (handle && desc.bcdUSB >= 0x0201)
			print_bos(handle);
	}
}

/* data_tx shall contains USB3_EP1_BULK_BURST_SIZE
   It is mandatory to send always USB3_EP1_BULK_BURST_SIZE (unused data are ignored)
   return 1 if ok or error code
*/
int usb_write_EP1(libusb_device_handle *handle, uint8_t* data_tx)
{
	int e = 0;
	int transferred = 0;

	e = libusb_bulk_transfer(handle, BULK_EP1_OUT, data_tx, USB3_EP1_BULK_BURST_SIZE, &transferred, LIBUSB_BULK_TRANSFER_TIMEOUT_MILLISEC); // Write
	if(e == 0) // write successfully
	{
		if (transferred == USB3_EP1_BULK_BURST_SIZE)
		{
			/*
			For USB3 CH569W (Especially with DEBUG=1 (serial port enabled with debug logs))
			Wait at least 1ms (else no data will be returned) that all data are received and command is executed
			*/
			sleep_us(1000);
			return 1;
		}
		else
		{
			log_printf_dbg("*** USB Bulk Write(libusb_bulk_transfer) error transferred=%u (transferred shall be %u) = 0 ***\n", transferred, USB3_EP1_BULK_BURST_SIZE);
			return -1;
		}
	}
	else
	{
		// write operation failed
		log_printf_dbg("USB Bulk Write(libusb_bulk_transfer) return error=%d\n", e);
		return -2;
	}
}
/* data_rx will contains USB3_EP1_BULK_BURST_SIZE bytes
   It is mandatory to receive always USB3_EP1_BULK_BURST_SIZE bytes (unused data are ignored)
   return 1 if ok or error code
*/
int usb_read_EP1(libusb_device_handle *handle, uint8_t* data_rx)
{
	int e = 0;
	int received = 0;

	e = libusb_bulk_transfer(handle, BULK_EP1_IN, data_rx, USB3_EP1_BULK_BURST_SIZE, &received, LIBUSB_BULK_TRANSFER_TIMEOUT_MILLISEC);  // Read
	if(e == 0) // Receive successfully
	{
		if (received == USB3_EP1_BULK_BURST_SIZE)
		{
			return 1;
		}
		else
		{
			log_printf_dbg("*** USB Bulk Read(libusb_bulk_transfer) error received=%u (received shall be %u) = 0 ***\n", received, USB3_EP1_BULK_BURST_SIZE);
			return -1;
		}
	}
	else
	{
		// read operation failed
		log_printf_dbg("USB Bulk Read(libusb_bulk_transfer) return error=%d\n", e);
		return -2;
	}
}

/*
   return 1 if ok or error code
*/
int usb_write_EP2(libusb_device_handle *handle, uint32_t* data_tx, int data_size_byte)
{
	int e = 0;
	int transferred = 0;

	e = libusb_bulk_transfer(handle, BULK_EP2_OUT, (uint8_t*)data_tx, data_size_byte, &transferred, LIBUSB_BULK_TRANSFER_TIMEOUT_MILLISEC); // Write
	if(e == 0) // write successfully
	{
		if (transferred == data_size_byte)
		{
			return 1;
		}
		else
		{
			log_printf_dbg("*** USB Bulk Write(libusb_bulk_transfer) error transferred=%u (transferred shall be %u) = 0 ***\n", transferred, data_size_byte);
			return -1;
		}
	}
	else
	{
		// write operation failed
		log_printf_dbg("USB Bulk Write(libusb_bulk_transfer) return error=%d\n", e);
		return -2;
	}
}
/*
   return 1 if ok or error code
*/
int usb_read_EP2(libusb_device_handle *handle, uint32_t* data_rx, int data_size_byte)
{
	int e = 0;
	int received = 0;

	e = libusb_bulk_transfer(handle, BULK_EP2_IN, (uint8_t*)data_rx, data_size_byte, &received, LIBUSB_BULK_TRANSFER_TIMEOUT_MILLISEC);  // Read
	if(e == 0) // Receive successfully
	{
		if (received == data_size_byte)
		{
			return 1;
		}
		else
		{
			log_printf_dbg("*** USB Bulk Read(libusb_bulk_transfer) error received=%u (received shall be %u) = 0 ***\n", received, data_size_byte);
			return -1;
		}
	}
	else
	{
		// read operation failed
		log_printf_dbg("USB Bulk Read(libusb_bulk_transfer) return error=%d\n", e);
		return -2;
	}
}

/*
Return 0 if error or struct libusb_device_handle * if success
*/
struct libusb_device_handle* usb_opendev(int verbose)
{
	int r = 1;
	struct libusb_device **devs;
	struct libusb_device *dev;

	struct libusb_device_descriptor desc;

	ssize_t cnt;
	int e = 0;
	int i = 0;
	char found = 0;

	// Get a list of USB devices
	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0)
	{
		log_printf("libusb_opendev() error there are no USB devices on the bus\n");
		return NULL;
	}
	//log_printf("\nDevice count: %zu\n-------------------------------\n", cnt);

	while ((dev = devs[i++]) != NULL)
	{
		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
		{
			log_printf("libusb_opendev() error failed to get device descriptor\n");
			libusb_free_device_list(devs, 1);
			break;
		}

		e = libusb_open(dev, &handle);
		if (e < 0)
		{
			//log_printf("Error opening device%d\n", i);
			continue;
		}

		if(desc.idVendor == VID && desc.idProduct == PID) // VID & PID found
		{
			if (desc.iSerialNumber)
			{
#define MAX_STRING_LEN (256)
				char string[MAX_STRING_LEN+1] = { 0 };
				int string_len;
				string_len = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char*)string, MAX_STRING_LEN);
				if(string_len == USB_SERIAL_NUMBER_LEN_EXPECTED)
				{
					if(strncmp((const char*)string, USB_SERIAL_HEADER_EXPECTED, USB_SERIAL_HEADER_EXPECTED_LEN) == 0)
					{
						print_device(dev, handle, verbose);
						found = 1;
					}
					else
					{
						log_printf("Error USB SerialNumber content(%.13s) is different from expected data(%.13s)\n", string, USB_SERIAL_HEADER_EXPECTED);
					}
				}
				else
				{
					log_printf("Error USB SerialNumber(%s) len=%d is different from expected len=%d\n", string, string_len, USB_SERIAL_NUMBER_LEN_EXPECTED);
				}
			}
			else
			{
				log_printf("Error no USB SerialNumber descriptor available\n");
			}
			break;
		}
		else // VID & PID not found
		{
			if(handle != NULL)
			{
				usb_closedev(handle);
			}
		}
	}//end of while

	if(found == 0)
	{
		log_printf("libusb_opendev() error device with VID:0x%04X PID:0x%04X not found\n", VID, PID);
		libusb_free_device_list(devs, 1);
		if(handle != NULL)
		{
			usb_closedev(handle);
		}
		return NULL;
	}

	libusb_free_device_list(devs, 1);

	if(libusb_kernel_driver_active(handle, 0) == 1)
	{
		log_printf("Kernel Driver Active\n");
		if(libusb_detach_kernel_driver(handle, 0) == 0)
		{
			log_printf("Kernel Driver Detached!\n");
		}
		else
		{
			log_printf("libusb_opendev() error could not detach kernel driver!\n");
			libusb_free_device_list(devs, 1);
			if(handle != NULL)
			{
				usb_closedev(handle);
			}
			return NULL;
		}
	}

	e = libusb_claim_interface(handle, 0);
	if(e < 0)
	{
		log_printf("libusb_opendev() error cannot claim interface\n");
		libusb_free_device_list(devs, 1);
		if(handle != NULL)
		{
			usb_closedev(handle);
		}
		return NULL;
	}
	else
	{
		usb_claim_interface = 1;
		return handle;
	}
}

void usb_closedev(struct libusb_device_handle* libusb_dev_handle)
{
	if(libusb_dev_handle != NULL)
	{
		if(usb_claim_interface == 1)
		{
			libusb_release_interface(libusb_dev_handle, 0);
			usb_claim_interface = 0;
		}
		libusb_close(libusb_dev_handle);
		handle = NULL;
	}
}

int usb_init(void)
{
	int r;
	const struct libusb_version* version;
	version = libusb_get_version();
	log_printf("Using libusb v%d.%d.%d.%d\n", version->major, version->minor, version->micro, version->nano);

	// Init libusb
	r = libusb_init(NULL);
	if(r < 0)
	{
		log_printf("libusb_init() error failed to initialise libusb\n");
	}
	return r;
}

void usb_exit(void)
{
	libusb_exit(NULL);
}

enum libusb_speed usb_get_device_speed(libusb_device_handle *handle)
{
	return usb_speed;
}

/* Return 0 if success other value are error code */
int USB_TestDataIntegrity(struct libusb_device_handle *handle, uint32_t *writebuf, uint32_t *readbuf, int test_num)
{
	int test_data_len;
	size_t mTotal = 0;
	uint32_t test_data_start_val = 0;
	int mTestCount;
	double speed_mbytes_per_sec;
	float time_diff_s;

	struct timeval start_data;
	struct timeval curr_data;

	if(usb_speed == LIBUSB_SPEED_SUPER)
	{
		test_data_len = USB3_EP2_BULK_BURST_SIZE; /* USB3_EP2_BULK_BURST_SIZE Bytes for USB3 SS */
	}
	else
	{
		test_data_len = USB2_BULK_SIZE; /* 512 Bytes for USB2 HS */
	}

	log_printf("Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests\n");
	log_printf("libusb_bulk_transfer(WriteEP=0x%02X)/libusb_bulk_transfer(ReadEP=0x%02X)/memcmp: %d times %d Bytes ***\n", BULK_EP2_OUT, BULK_EP2_IN, test_num, test_data_len);

	log_printf_dbg("Start Test\n");
	gettimeofday(&start_data, NULL);
	for (mTestCount = 0; mTestCount < test_num; ++mTestCount) // loop test
	{
		uint32_t* memWBuf = (uint32_t*)writebuf;
		for (uint32_t i = 0; i < (test_data_len/4); i++)
		{
			memWBuf[i] = swap_uint32(i+test_data_start_val); // Swap from LE to BE
		}
		test_data_start_val += (test_data_len/4);

		if(usb_write_EP2(handle, writebuf, test_data_len) == 1)
		{
			mTotal += test_data_len;
		}
		else
		{
			log_printf("usb_write_EP2() error\n");
			log_printf("Test failure end with error\n");
			return -2;
		}

		memset(readbuf, 0x00, test_data_len);
		if(usb_read_EP2(handle, readbuf, test_data_len) == 1)
		{
			mTotal += test_data_len;
		}
		else
		{
			log_printf("usb_read_EP2() error\n");
			log_printf("Test failure end with error\n");
			return -2;
		}

		if(memcmp(readbuf, writebuf, test_data_len) != 0)
		{
			log_printf("memcmp() failed mTestCount=%d\n", mTestCount);
			log_printf("Test failure end with error\n");
			return -2;
		}
	}
	gettimeofday(&curr_data, NULL);
	log_printf_dbg("End Test\n");

	time_diff_s = TimevalDiff(&curr_data, &start_data);
	speed_mbytes_per_sec = (float)(((double)mTotal) / (1000.0 * 1000.0)) / time_diff_s;
	log_printf("Average speed %.1f MBytes/Sec, Total=%zu Bytes/%zu MBytes\n", speed_mbytes_per_sec, mTotal, (mTotal/(1000 * 1000)));
	log_printf("End USB_TestDataIntegrity(libusb_bulk_transfer) Tests\n");
	log_printf("Test end with success\n");

	return 0;
}

/* Return 0 if success other value are error code */
int USB_TestDataSpeed(struct libusb_device_handle *handle, uint32_t *writebuf, uint32_t *readbuf, int test_num, int test_data_len)
{
	size_t mTotal;
	uint32_t test_data_start_val = 0x11001100;
	int mTestCount;
	double speed_mbytes_per_sec;

	struct timeval start_data;
	struct timeval curr_data;

	float time_diff_s;

	log_printf("Start USB_TestDataSpeed(libusb_bulk_transfer) Tests\n");
	log_printf("libusb_bulk_transfer(WriteEP=0x%02X)/libusb_bulk_transfer(ReadEP=0x%02X): %d times %d Bytes ***\n", BULK_EP2_OUT, BULK_EP2_IN, test_num, test_data_len);

	uint32_t* memWBuf = writebuf;
	for (uint32_t i = 0; i < test_data_len / 4; i++)
	{
		memWBuf[i] = swap_uint32(i+test_data_start_val); // Swap from LE to BE
	}

	log_printf_dbg("Start Test1 USB Bulk Write(libusb_bulk_transfer)\n");
	mTotal = 0;
	gettimeofday(&start_data, NULL);
	for (mTestCount = 0; mTestCount < test_num; ++mTestCount) // loop test
	{
		if(usb_write_EP2(handle, writebuf, test_data_len) == 1)
		{
			mTotal += test_data_len;
		}
		else
		{
			log_printf_dbg("usb_write_EP2() error\n");
			log_printf_dbg("Test failure end with error\n");
			return -2;
		}
	}
	gettimeofday(&curr_data, NULL);
	log_printf_dbg("End Test\n");

	time_diff_s = TimevalDiff(&curr_data, &start_data);
	speed_mbytes_per_sec = (float)(((double)mTotal) / (1000.0 * 1000.0)) / time_diff_s;
	log_printf("Average speed %.1f MBytes/Sec, Total=%zu Bytes/%zu MBytes\n", speed_mbytes_per_sec, mTotal, (mTotal/(1000 * 1000)));

	log_printf_dbg("Start Test2 USB Bulk Read(libusb_bulk_transfer)\n");
	mTotal = 0;
	gettimeofday(&start_data, NULL);
	for (mTestCount = 0; mTestCount < test_num; ++mTestCount) // loop test
	{
		if(usb_read_EP2(handle, readbuf, test_data_len) == 1)
		{
			mTotal += test_data_len;
		}
		else
		{
			log_printf_dbg("usb_read_EP2() error\n");
			log_printf_dbg("Test failure end with error\n");
			return -2;
		}
	}
	gettimeofday(&curr_data, NULL);
	log_printf_dbg("End Test\n");

	time_diff_s = TimevalDiff(&curr_data, &start_data);
	speed_mbytes_per_sec = (float)(((double)mTotal) / (1000.0 * 1000.0)) / time_diff_s;
	log_printf("Average speed %.1f MBytes/Sec, Total=%zu Bytes/%zu MBytes\n", speed_mbytes_per_sec, mTotal, (mTotal/(1000 * 1000)));

	log_printf("End USB_TestDataSpeed(libusb_bulk_transfer) Tests\n");
	log_printf("Test end with success\n");
	return 0;
}
