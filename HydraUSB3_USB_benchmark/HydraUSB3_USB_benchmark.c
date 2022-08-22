/********************************** (C) COPYRIGHT *******************************
* File Name          : HydraUSB3_USB_benchmark.c
* Author             : bvernoux
* Version            : V1.0.1
* Date               : 2022/08/22
* Description        :
* Copyright (c) 2022 Benjamin VERNOUX
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>

#include <cargs.h>

#include "libusb_portable.h"
#include "usb_cmd.h"

struct libusb_device_handle *handle = NULL;
int usb_speed;

/* Small Buffer for EP1 (Command) */
static uint8_t mReadBuf[ (USB3_EP1_BULK_BURST_SIZE+4) ];
static uint8_t mWriteBuf[ (USB3_EP1_BULK_BURST_SIZE+4) ];

/* Big Buffer for EP2 (Benchmark) */
#define TEST_INTEGRITY_NUM 10000
#define TEST_NUM_SS 100 // Number of test for USB3 Super Speed(5Gbit/s)
#define TEST_NUM_HS 10 // Number of test for USB2 High Speed(480Mbit/s)
#define TEST_DATA_LEN 0x800000 // 8 MiB
//#define TEST_DATA_LEN 0x1000000 // 16 MiB fail on some PC with libusb (Can return libusb_bulk_transfer return error=-11 => NO MEM)
static __attribute__ ((aligned (128))) uint32_t mReadBigBuf[ ((TEST_DATA_LEN+16)/sizeof(uint32_t)) ];
static __attribute__ ((aligned (128))) uint32_t mWriteBigBuf[ ((TEST_DATA_LEN+16)/sizeof(uint32_t)) ];

FILE *pFile = NULL; /* File for logging */

void cleanup(void)
{
	if(pFile != NULL)
	{
		fclose(pFile);
	}
	if(handle != NULL)
	{
		libusb_release_interface(handle, 0);
		libusb_close(handle);
	}
}

void error_exit(char* error_str)
{
	if(error_str != NULL)
	{
		log_printf("%sTests end with failure(s)\n", error_str);
	}
	cleanup();
	log_printf("Press Enter to exit\n");
	getchar();
	libusb_exit(NULL);
	exit(-1);
}

#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT)
	{
		log_printf("\nCtrl-C pressed\nExit\n");
		error_exit(NULL);
	}
	return TRUE;
}
#else
void consoleHandler(int s)
{
	if (s == SIGINT) // Ctrl-C
	{
		log_printf("\nCtrl-C pressed\nExit\n");
		error_exit(NULL);
	}
}
#endif

/**
 * This is the main configuration of all options available.
 */
static struct cag_option options[] =
{
	{
		.identifier = 'v',
		.access_letters = "v",
		.access_name = "verbose",
		.value_name = "VALUE",
		.description = "Verbose mode to have more detais on output"
	},

	{
		.identifier = 'h',
		.access_letters = "h",
		.access_name = "help",
		.description = "Shows the command help"
	}
};

/**
 * This is a custom project configuration structure where you can store the
 * parsed information.
 */
struct cag_configuration
{
	int verbose;
};

int main(int argc, char *argv[])
{
	char identifier;
	const char *value;
	cag_option_context context;
	struct cag_configuration config = {false }; /* Default values for the option(s) */

	int usb2_error = 1;
	int usb3_error = 1;
	uint8_t *data_tx, *data_rx;

#define DATETIME_STR_SIZE (30)
	char datetime_str[DATETIME_STR_SIZE + 1] = "";
#define FILENAME_SIZE (30+100)
	char filename[FILENAME_SIZE + 1] = "";
	time_t rawtime;
	struct tm* timeinfo;

	uint32_t* memWBuf = (uint32_t*)mWriteBuf;

	/**
	* Prepare the context and iterate over all options.
	*/
	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
	while (cag_option_fetch(&context))
	{
		identifier = cag_option_get(&context);
		switch (identifier)
		{
			case 'v':
				value = cag_option_get_value(&context);
				config.verbose = atoi(value);
				break;
			case 'h':
				printf("Usage: HydraUSB3_USB_benchmark [OPTION]...\n");
				cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
				return EXIT_SUCCESS;
		}
	}

#ifdef _WIN32
	if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
	{
		fprintf(stderr, "\nERROR: Could not set control handler\n");
		fflush(stderr);

		if(stdout)
		{
			fprintf(stdout, "\nERROR: Could not set control handler\n");
			fflush(stdout);
		}
		exit(-1);
	}
#else
	signal(SIGINT, consoleHandler);
#endif

	//memset(memWBuf, 0, USB3_BULK_BURST_SIZE);
	for (uint32_t i = 0; i < sizeof(mWriteBuf) / 4; i++)
	{
		memWBuf[i] = swap_uint32(i); // Swap from LE to BE
	}

	data_rx = &mReadBuf[0];
	data_tx = &mWriteBuf[0];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(datetime_str, DATETIME_STR_SIZE, "%Y%m%d_%H%M%S", timeinfo);
	snprintf(filename, FILENAME_SIZE, "hydrausb3_usb_benchmark_%s.txt", datetime_str);
	pFile = fopen(filename, "w");
	if(pFile == NULL)
	{
		log_printf("fopen(filename, \"w\") error (filename=\"%s\")\n", filename);
		error_exit(NULL);
	}
	log_printf_init(pFile);

	log_printf("HydraUSB3_USB_benchmark v%s B.VERNOUX 22-Aug-2022\n", VERSION);

	log_printf("Options: verbose=%d\n",
			   config.verbose);

	log_printf("USB3_EP1_BULK_BURST_SIZE=%d USB3_EP2_BULK_BURST_SIZE=%d\n", USB3_EP1_BULK_BURST_SIZE, USB3_EP2_BULK_BURST_SIZE);


	handle = usb_opendev(config.verbose);
	if(handle == NULL)
	{
		error_exit("usb_opendev() error exit\n");
	}
	usb_speed = usb_get_device_speed(handle);
	if(usb_speed == LIBUSB_SPEED_SUPER)
	{
		log_printf("Test end with success\n");
	}
	else
	{
		error_exit("Error HydraUSB3 device shall be connected with SuperSpeed 5Gbit/s Computer/Cable(no HUB) to start the tests\n");
	}
	log_printf("\n");

	/* USB3 SS Test Data Integrity */
	if(USB_TestDataIntegrity(handle, mWriteBigBuf, mReadBigBuf, TEST_INTEGRITY_NUM) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");

	/* USB3 SS Test Data Speed */
	if(USB_TestDataSpeed(handle, mWriteBigBuf, mReadBigBuf, TEST_NUM_SS, TEST_DATA_LEN) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");

	log_printf_dbg("Start USB2 HS Force\n");
	{
		/* Write command to Get USB STATUS */
		memWBuf[0] = USB_CMD_USB2;
		usb_write_EP1(handle, data_tx);
	}
	/* Wait to be sure the command is executed */
	sleep_ms(10);
	/* Release and close the handle as USB enumeration is done again for USB2 HS */
	libusb_release_interface(handle, 0);
	libusb_close(handle);
	/* Wait USB2 HS enumeration */
	sleep_ms(800);
	handle = usb_opendev(config.verbose);
	log_printf_dbg("End USB2 HS Force\n");
	if(handle == NULL)
	{
		error_exit("Error to open HydraUSB3 device, it is expected to be enumerated as USB2 HighSpeed(480Mbit)\n");
	}
	usb_speed = usb_get_device_speed(handle);
	if(usb_speed == LIBUSB_SPEED_HIGH)
	{
		log_printf("Test end with success\n");
	}
	else
	{
		error_exit("Error HydraUSB3 device shall be enumerated as USB2 HighSpeed(480Mbit)\n");
	}
	log_printf("\n");

	/* USB2 HS Test Data Integrity */
	if(USB_TestDataIntegrity(handle, mWriteBigBuf, mReadBigBuf, TEST_INTEGRITY_NUM) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");
	/* USB2 HS Test Data Speed */
	if(USB_TestDataSpeed(handle, mWriteBigBuf, mReadBigBuf, TEST_NUM_HS, TEST_DATA_LEN) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");

	log_printf_dbg("Start Read USB2 Status\n");
	{
		/* Write command to Get USB STATUS */
		memWBuf[0] = USB_CMD_USBS;
		usb_write_EP1(handle, data_tx);
		/* Read USB Status Report */
		if(usb_read_EP1(handle, data_rx) == 1)
		{
			data_rx[USB3_EP1_BULK_BURST_SIZE] = 0; // Safety terminate string at end of buffer
			log_printf("%s", data_rx);
			log_printf("\n");
			if(strstr((char*)data_rx, "USB2 SPEED=1") != NULL)
			{
				usb2_error = 0;
			}
			else
			{
				log_printf("Error USB2 SPEED != 1\n");
				log_printf("Test end with failure\n");
			}
		}
		else
		{
			error_exit("Error usb_read_EP1() USB_CMD_USBS\n");
		}
	}

	log_printf_dbg("Start USB3 Force\n");
	{
		/* Write command to Get USB STATUS */
		memWBuf[0] = USB_CMD_USB3;
		usb_write_EP1(handle, data_tx);
	}
	/* Wait to be sure the command is executed */
	sleep_ms(10);
	/* Release and close the handle as USB enumeration is done again for USB2 HS */
	libusb_release_interface(handle, 0);
	libusb_close(handle);
	/* Wait USB2 HS enumeration */
	sleep_ms(800);
	handle = usb_opendev(config.verbose);
	log_printf_dbg("End USB3 Force\n");
	if(handle == NULL)
	{
		error_exit("Error to open HydraUSB3 device, it is expected to be enumerated as USB3 SuperSpeed(5Gbit)\n");
	}
	usb_speed = usb_get_device_speed(handle);
	if(usb_speed == LIBUSB_SPEED_SUPER)
	{
		log_printf("Test end with success\n");
	}
	else
	{
		error_exit("Error HydraUSB3 device shall be enumerated as USB3 SuperSpeed(5Gbit)\n");
	}
	log_printf("\n");

	/* USB3 SS Test Data Integrity */
	if(USB_TestDataIntegrity(handle, mWriteBigBuf, mReadBigBuf, TEST_INTEGRITY_NUM) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");

	/* USB3 SS Test Data Speed */
	if(USB_TestDataSpeed(handle, mWriteBigBuf, mReadBigBuf, TEST_NUM_SS, TEST_DATA_LEN) != 0)
	{
		error_exit(NULL);
	}
	log_printf("\n");

	log_printf_dbg("Read Log\n");
	{
		/* Write command to Read Log */
		memWBuf[0] = USB_CMD_LOGR; /* Read Log command */
		usb_write_EP1(handle, data_tx);
		/* Read Log */
		if(usb_read_EP1(handle, data_rx) == 1)
		{
			data_rx[USB3_EP1_BULK_BURST_SIZE] = 0; // Safety terminate string at end of buffer
			//print_hex(data_rx, USB3_EP1_BULK_BURST_SIZE);
			log_printf("%s", data_rx);
			log_printf("\n");
		}
		else
		{
			error_exit("Error usb_read_EP1() USB_CMD_LOGR\n");
		}
	}

	log_printf_dbg("Start Read USB3 Status\n");
	{
		/* Write command to Get USB STATUS */
		memWBuf[0] = USB_CMD_USBS;
		usb_write_EP1(handle, data_tx);
		/* Read USB Status Report */
		if(usb_read_EP1(handle, data_rx) == 1)
		{
			data_rx[USB3_EP1_BULK_BURST_SIZE] = 0; // Safety terminate string at end of buffer
			log_printf("%s", data_rx);
			log_printf("\n");
			if(strstr((char*)data_rx, "LINK_ERR_CNT=0x00000000") != NULL)
			{
				usb3_error = 0;
			}
			else
			{
				log_printf("Error USB_CMD_USBS LINK_ERR_CNT != 0\n");
				log_printf("Test end with failure\n");
			}
		}
		else
		{
			error_exit("Error usb_read_EP1() USB_CMD_USBS\n");
		}
	}
	log_printf("\n");

	log_printf_dbg("Read Log\n");
	{
		/* Write command to Read Log */
		memWBuf[0] = USB_CMD_LOGR; /* Read Log command */
		usb_write_EP1(handle, data_tx);
		/* Read Log */
		if(usb_read_EP1(handle, data_rx) == 1)
		{
			data_rx[USB3_EP1_BULK_BURST_SIZE] = 0; // Safety terminate string at end of buffer
			//print_hex(data_rx, USB3_EP1_BULK_BURST_SIZE);
			log_printf("%s", data_rx);
			log_printf("\n");
		}
		else
		{
			error_exit("Error usb_read_EP1() USB_CMD_LOGR\n");
		}
	}

	/* Summary */
	if( (usb2_error == 0) && (usb3_error == 0) )
	{
		log_printf("Tests end with success\n");
	}
	else
	{
		log_printf("Tests end with failure(s)\n");
	}

	/* Reboot the board */
	/* Write command to Reboot the board */
	memWBuf[0] = USB_CMD_BOOT;
	usb_write_EP1(handle, data_tx);
	/* Wait to be sure the command is executed */
	sleep_ms(500);

	cleanup();
	libusb_exit(NULL);

	return EXIT_SUCCESS;
}
