/********************************** (C) COPYRIGHT *******************************
* File Name          : usb_cmd.h
* Author             : bvernoux
* Version            : V1.0
* Date               : 2022/08/07
* Description        :
* Copyright (c) 2022 Benjamin VERNOUX
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

#ifndef USB_CMD_H_
#define USB_CMD_H_

/* USB2 or USB3 commands from Host to Device */
#define USB_CMD_LOGR (0x4C4F4752) // CMD LOGR (Return LOG)
#define USB_CMD_USBS (0x55534253) // CMD USBS (Return USB Status log)
#define USB_CMD_USB2 (0x55534232) // CMD USB2 (Switch to USB2 even if USB3 is available)
#define USB_CMD_USB3 (0x55534233) // CMD USB3 (Switch to USB3 or do a fall-back to USB2 if not available)
#define USB_CMD_BOOT (0x424F4F54) // CMD BOOT (Reboot the board)

#endif /* USB_CMD_H_ */
