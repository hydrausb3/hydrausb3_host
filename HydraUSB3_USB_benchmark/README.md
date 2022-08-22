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
HydraUSB3_USB_benchmark v1.0.1 B.VERNOUX 22-Aug-2022
Options: verbose=0
USB3_EP1_BULK_BURST_SIZE=4096 USB3_EP2_BULK_BURST_SIZE=4096
Using libusb v1.0.26.11724
2022-08-22 12:21:32.732 (0.209 s) Dev (bus 2, device 9): 16C0 - 05DC speed: 5G
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 4096 Bytes ***
2022-08-22 12:21:32.752 (0.228 s) Start Test
2022-08-22 12:21:34.276 (1.753 s) End Test
Average speed 53.7 MBytes/Sec, Total=81920000 Bytes/81 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-22 12:21:34.302 (1.779 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-22 12:21:37.450 (4.927 s) End Test
Average speed 266.9 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-22 12:21:37.466 (4.943 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-22 12:21:39.958 (7.435 s) End Test
Average speed 336.6 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-22 12:21:39.974 (7.451 s) Start USB2 HS Force
Using libusb v1.0.26.11724
2022-08-22 12:21:41.006 (8.482 s) Dev (bus 2, device 10): 16C0 - 05DC speed: 480M
2022-08-22 12:21:41.006 (8.482 s) End USB2 HS Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 512 Bytes ***
2022-08-22 12:21:41.026 (8.502 s) Start Test
2022-08-22 12:21:42.721 (10.198 s) End Test
Average speed 6.0 MBytes/Sec, Total=10240000 Bytes/10 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 10 times 8388608 Bytes ***
2022-08-22 12:21:42.752 (10.229 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-22 12:21:44.480 (11.957 s) End Test
Average speed 48.6 MBytes/Sec, Total=83886080 Bytes/83 MBytes
2022-08-22 12:21:44.480 (11.957 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-22 12:21:48.139 (15.615 s) End Test
Average speed 22.9 MBytes/Sec, Total=83886080 Bytes/83 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-22 12:21:48.154 (15.631 s) Start Read USB2 Status
USBS USB2:
USB2 SPEED=1 (0=FS,1=HS,2=LS)
Test end with success

2022-08-22 12:21:48.154 (15.631 s) Start USB3 Force
Using libusb v1.0.26.11724
2022-08-22 12:21:49.173 (16.649 s) Dev (bus 2, device 11): 16C0 - 05DC speed: 5G
2022-08-22 12:21:49.182 (16.659 s) End USB3 Force
Test end with success

Start USB_TestDataIntegrity(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82)/memcmp: 10000 times 4096 Bytes ***
2022-08-22 12:21:49.192 (16.669 s) Start Test
2022-08-22 12:21:50.698 (18.175 s) End Test
Average speed 54.4 MBytes/Sec, Total=81920000 Bytes/81 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

Start USB_TestDataSpeed(libusb_bulk_transfer) Tests
libusb_bulk_transfer(WriteEP=0x02)/libusb_bulk_transfer(ReadEP=0x82): 100 times 8388608 Bytes ***
2022-08-22 12:21:50.714 (18.190 s) Start Test1 USB Bulk Write(libusb_bulk_transfer)
2022-08-22 12:21:53.877 (21.353 s) End Test
Average speed 266.5 MBytes/Sec, Total=838860800 Bytes/838 MBytes
2022-08-22 12:21:53.946 (21.422 s) Start Test2 USB Bulk Read(libusb_bulk_transfer)
2022-08-22 12:21:56.453 (23.930 s) End Test
Average speed 336.6 MBytes/Sec, Total=838860800 Bytes/838 MBytes
End USB_TestDataIntegrity(libusb_bulk_transfer) Tests
Test end with success

2022-08-22 12:21:56.469 (23.946 s) Read Log
00s 000ms 000us Start
00s 000ms 041us ChipID(Hex)=69
00s 000ms 096us FLASH_ROMA_UID(Hex)=84 97 3B 26 3B 38 FA F5
00s 000ms 215us HydraUSB3_USB FW v1.0.1 22-Aug-2022(CPU Freq=120 MHz)
00s 000ms 276us DEF_ENDP1_MAX_SIZE=4096 DEF_ENDP2_MAX_SIZE=4096
00s 018ms 242us USB3
08s 846ms 478us cmd USB2
08s 846ms 518us Force USB2
09s 048ms 528us USB2
17s 021ms 088us cmd USBS USB2
17s 029ms 462us cmd USB3
17s 029ms 503us Force USB3
17s 048ms 572us USB3
25s 335ms 306us cmd LOGR

2022-08-22 12:21:56.500 (23.977 s) Start Read USB3 Status
USBS USB3:
LINK_STATUS=0x00000109
LINK_ERR_STATUS=0x00000000
LINK_ERR_CNT=0x00000000

2022-08-22 12:21:56.516 (23.992 s) Read Log
25s 372ms 631us cmd USBS USB3
25s 383ms 604us cmd LOGR

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
