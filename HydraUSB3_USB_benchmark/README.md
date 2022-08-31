HydraUSB3_USB_benchmark
=======

HydraUSB3_USB_benchmark is a small utility to execute commands and do benchmarck on USB2 High Speed and USB3 Super Speed firmware with [HydraUSB3_USB](https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_USB) Firmware.

This example use libusb to exchange commands/data over USB2 High Speed and USB3 Super Speed (Device Bulk) with [HydraUSB3_USB](https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_USB) Firmware:
* `HydraUSB3_USB_benchmark` use following USB commands (using libusb)
  * `USB_CMD_LOGR` : Returns internal logs data over USB2/USB3 (log_printf()/cprintf())
  * `USB_CMD_USBS` : Return USB status of actual used USB (USB2 or USB3)
  * `USB_CMD_USB2` : Switch to USB2 even if USB3 is available
  * `USB_CMD_USB3` : Switch to USB3 or do a fall-back to USB2 if not available
  * `USB_CMD_BOOT` : Reboot the board
* USB Bulk Endpoints configuration
  * Endpoint1 is used for command/answer with 4KiB buffer(IN) and  4KiB buffer(OUT)
    * This Endpoint use 4 burst over USB3 (4KiB)
  * Endpoint2 is used for fast USB streaming with 4KiB buffers(IN/OUT)
    * This Endpoint use 4 burst over USB3 (4KiB)
* This example work over USB2/USB3 and is fully compatible with Linux and Windows8 or more (tested on Windows10 & Windows11)

This host tools has been tested with HydraUSB3 v1 + Firmware [HydraUSB3_USB](https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_USB)

Example output of `HydraUSB3_USB_benchmark` on Windows10 (Asus N56VZ Corei7-3630QM CPU@2.40GHz):
```
HydraUSB3_USB_benchmark v1.0.2 B.VERNOUX 31-Aug-2022
Options: verbose=0
USB3_EP1_BULK_BURST_SIZE=4096 USB3_EP2_BULK_BURST_SIZE=4096
Using libusb v1.0.26.11724
2022-08-31 06:09:21.362 (0.207 s) Dev (bus 2, device 10): 16C0 - 05DC speed: 5G
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 4096 Bytes ***
2022-08-31 06:09:21.378 (0.222 s) Start Test
2022-08-31 06:09:22.917 (1.762 s) End Test
Average speed 53.2 MBytes/Sec, Total=81920000 Bytes/81 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-31 06:09:22.940 (1.784 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-31 06:09:26.112 (4.957 s) End Test
Average speed 264.4 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-31 06:09:26.112 (4.957 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-31 06:09:28.620 (7.465 s) End Test
Average speed 336.6 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataSpeed(libusb_bulk_transfer) Tests
Test end with success

2022-08-31 06:09:28.635 (7.480 s) Start USB2 HS Force
2022-08-31 06:09:29.654 (8.499 s) Dev (bus 2, device 11): 16C0 - 05DC speed: 480M
2022-08-31 06:09:29.664 (8.508 s) End USB2 HS Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 512 Bytes ***
2022-08-31 06:09:29.683 (8.528 s) Start Test
2022-08-31 06:09:31.363 (10.208 s) End Test
Average speed 6.1 MBytes/Sec, Total=10240000 Bytes/10 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 10 times 8388608 Bytes ***
2022-08-31 06:09:31.395 (10.239 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-31 06:09:33.122 (11.967 s) End Test
Average speed 48.6 MBytes/Sec, Total=83886080 Bytes/83 MBytes
2022-08-31 06:09:33.122 (11.967 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-31 06:09:36.781 (15.626 s) End Test
Average speed 23.0 MBytes/Sec, Total=83886080 Bytes/83 MBytes
End USB_TestDataSpeed(libusb_bulk_transfer) Tests
Test end with success

2022-08-31 06:09:36.796 (15.641 s) Start Read USB2 Status
USBS USB2:
USB2 SPEED=1 (0=FS,1=HS,2=LS)
Test end with success

2022-08-31 06:09:36.796 (15.641 s) Start USB3 Force
2022-08-31 06:09:37.823 (16.668 s) Dev (bus 2, device 12): 16C0 - 05DC speed: 5G
2022-08-31 06:09:37.823 (16.668 s) End USB3 Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 4096 Bytes ***
2022-08-31 06:09:37.843 (16.688 s) Start Test
2022-08-31 06:09:39.338 (18.183 s) End Test
Average speed 54.8 MBytes/Sec, Total=81920000 Bytes/81 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-31 06:09:39.354 (18.199 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-31 06:09:42.517 (21.362 s) End Test
Average speed 266.5 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-31 06:09:42.533 (21.378 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-31 06:09:45.025 (23.870 s) End Test
Average speed 336.6 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataSpeed(libusb_bulk_transfer) Tests
Test end with success

2022-08-31 06:09:45.041 (23.886 s) Read Log
00s 000ms 000us Start
00s 000ms 041us ChipID(Hex)=69
00s 000ms 096us FLASH_ROMA_UID(Hex)=84 97 3B 26 3B 38 FA F5
00s 000ms 215us HydraUSB3_USB FW v1.0.1 22-Aug-2022(CPU Freq=120 MHz)
00s 000ms 276us DEF_ENDP1_MAX_SIZE=4096 DEF_ENDP2_MAX_SIZE=4096
00s 018ms 214us USB3
11s 012ms 243us cmd USB2
11s 012ms 283us Force USB2
11s 518ms 268us USB2
19s 170ms 645us cmd USBS USB2
19s 178ms 915us cmd USB3
19s 178ms 955us Force USB3
19s 518ms 312us USB3
27s 414ms 101us cmd LOGR

2022-08-31 06:09:45.072 (23.917 s) Start Read USB3 Status
USBS USB3:
LINK_STATUS=0x00000109
LINK_ERR_STATUS=0x00000000
LINK_ERR_CNT=0x00000000

2022-08-31 06:09:45.094 (23.939 s) Read Log
27s 458ms 451us cmd USBS USB3
27s 469ms 771us cmd LOGR

Tests end with success
```

For more details on HydraUSB3 v1 Hardware see https://github.com/hydrausb3/hydrausb3_hw
For more details on HydraUSB3 v1 Firmware see https://github.com/hydrausb3/hydrausb3_fw

### How to build HydraUSB3_USB_benchmark
- See the document [How_To_Build.md](How_To_Build.md)

### How to use HydraUSB3_USB_benchmark
- Start a shell(on GNU/Linux) or execute cmd(on Windows)
  - #### GNU/Linux
    - HydraUSB3_USB_benchmark
  - #### Windows
    - HydraUSB3_USB_benchmark.exe
