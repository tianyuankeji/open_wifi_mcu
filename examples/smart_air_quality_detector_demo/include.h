/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "stm32f10x.h"

#define DEVICE_INFO     0x0000
#define DVC_CHIP_ID     0x00C1
#define DVC_VCODE       0x00C4
#define DVC_PCODE       0x00C5
#define DVC_HWVSION     0x00C6
#define DVC_SWVSION     0x00C7
#define DVC_CON_WIFI    0x00D6

#define SWITCH_CLUSTER  0x0006

#define CHIP_ID         "000000"
#define VCODE           "TIANYUAN"
#define PCODE           "PLT002"
#define HW_VERSION      "MCU"
#define SW_VERSION      "TIANYUAN-PLT002-MCU-20171212-0"

#endif