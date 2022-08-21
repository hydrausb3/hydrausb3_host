HydraUSB3_USB_benchmark
=======

HydraUSB3_USB_benchmark is a small utility to do benchmarck on USB2 High Speed and USB3 Super Speed firmware with [HydraUSB3_USB](https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_USB) Firmware.

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
  * Endpoint2 is used for fast USB streaming with 8KiB buffers(IN/OUT)
    * This Endpoint use 8 burst over USB3 (8KiB)
* This example work over USB2/USB3 and is fully compatible with Linux and Windows8 or more (tested on Windows10 & Windows11)

This host tools has been tested with HydraUSB3 v1 + Firmware [HydraUSB3_USB](https://github.com/hydrausb3/hydrausb3_fw/tree/main/HydraUSB3_USB)

Example output of `HydraUSB3_USB_benchmark` on Windows10:
```
HydraUSB3_USB_benchmark v1.0.0 B.VERNOUX 20-Aug-2022
Options: verbose=0
USB3_EP1_BULK_BURST_SIZE=4096 USB3_EP2_BULK_BURST_SIZE=8192
Using libusb v1.0.26.11724
2022-08-21 17:29:58.989 (0.220 s) Dev (bus 2, device 5): 16C0 - 05DC speed: 5G
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 8192 Bytes ***
2022-08-21 17:29:59.009 (0.240 s) Start Test
2022-08-21 17:30:01.420 (2.650 s) End Test
Average speed 68.1 MBytes/Sec, Total=163840000 Bytes/163 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-21 17:30:01.444 (2.674 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-21 17:30:04.019 (5.249 s) End Test
Average speed 326.2 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-21 17:30:04.023 (5.253 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-21 17:30:06.556 (7.787 s) End Test
Average speed 331.7 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-21 17:30:06.564 (7.795 s) Start USB2 HS Force
Using libusb v1.0.26.11724
2022-08-21 17:30:07.638 (8.869 s) Dev (bus 2, device 6): 16C0 - 05DC speed: 480M
2022-08-21 17:30:07.646 (8.877 s) End USB2 HS Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 512 Bytes ***
2022-08-21 17:30:07.692 (8.923 s) Start Test
2022-08-21 17:30:09.427 (10.658 s) End Test
Average speed 5.9 MBytes/Sec, Total=10240000 Bytes/10 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 10 times 8388608 Bytes ***
2022-08-21 17:30:09.451 (10.682 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-21 17:30:11.182 (12.413 s) End Test
Average speed 48.6 MBytes/Sec, Total=83886080 Bytes/83 MBytes
2022-08-21 17:30:11.186 (12.417 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-21 17:30:14.768 (15.999 s) End Test
Average speed 23.4 MBytes/Sec, Total=83886080 Bytes/83 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-21 17:30:14.780 (16.011 s) Start Read USB2 Status
USBS USB2:
USB2 SPEED=1 (0=FS,1=HS,2=LS)
Test end with success

2022-08-21 17:30:14.788 (16.019 s) Start USB3 Force
Using libusb v1.0.26.11724
2022-08-21 17:30:15.806 (17.037 s) Dev (bus 2, device 7): 16C0 - 05DC speed: 5G
2022-08-21 17:30:15.814 (17.045 s) End USB3 Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 8192 Bytes ***
2022-08-21 17:30:15.851 (17.082 s) Start Test
2022-08-21 17:30:18.236 (19.467 s) End Test
Average speed 68.8 MBytes/Sec, Total=163840000 Bytes/163 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-21 17:30:18.261 (19.492 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-21 17:30:20.834 (22.065 s) End Test
Average speed 326.6 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-21 17:30:20.843 (22.074 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-21 17:30:23.365 (24.596 s) End Test
Average speed 333.1 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-21 17:30:23.381 (24.612 s) Read Log
00s 000ms 000us Start
00s 000ms 040us ChipID(Hex)=69
00s 000ms 094us FLASH_ROMA_UID(Hex)=84 97 3B 26 3B 38 FA F5
00s 000ms 210us HydraUSB3_USB FW v1.0 20-Aug-2022(CPU Freq=120 MHz)
00s 000ms 269us DEF_ENDP1_MAX_SIZE=4096 DEF_ENDP2_MAX_SIZE=8192
00s 018ms 116us USB3
31s 735ms 019us cmd USB2
31s 735ms 059us Force USB2
32s 018ms 187us USB2
39s 949ms 557us cmd USBS USB2
39s 956ms 662us cmd USB3
39s 956ms 702us Force USB3
40s 018ms 237us USB3
48s 549ms 941us cmd LOGR

2022-08-21 17:30:23.419 (24.650 s) Start Read USB3 Status
USBS USB3:
LINK_STATUS=0x00000109
LINK_ERR_STATUS=0x00000000
LINK_ERR_CNT=0x00000000

2022-08-21 17:30:23.427 (24.658 s) Read Log
48s 587ms 824us cmd USBS USB3
48s 595ms 838us cmd LOGR

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
