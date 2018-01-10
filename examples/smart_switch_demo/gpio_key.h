/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __GPIO_KEY_H__
#define __GPIO_KEY_H__

#include "stm32f10x.h"
#include "stdint.h"

#define GPIO_KEY1_PIN   GPIO_Pin_12
#define GPIO_KEY2_PIN   GPIO_Pin_13
#define GPIO_KEY3_PIN   GPIO_Pin_14

#define KEY_SPD                 0x00
#define KEY_DOWN                0x01
#define KEY_LONG_PRESS          0x02
#define KEY_UP                  0x04

#define KEY_SHORT_PRESS_TIME    50
#define KEY_LONG_PRESS_TIME     (3*1000)

extern uint8_t wifi_work_status;

void Key_detection(void);

uint8_t Key_init(void);

#endif