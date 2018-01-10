/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __GPIO_LIGHT_H__
#define __GPIO_LIGHT_H__

#include "stm32f10x.h"
#include "stdint.h"

#define WIFI_STATUS_LED  GPIO_Pin_1
#define KEY_RGB_B        GPIO_Pin_6
#define KEY_LIGHT_3      GPIO_Pin_0

#define KEY_LIGHT_1      GPIO_Pin_5
#define KEY_LIGHT_2      GPIO_Pin_6

#define SMART_CONFIG_START      2
#define WIFI_CONNECT_SUCCESS    0
#define WIFI_NOT_CONNECT        1

extern uint8_t key_press;
extern uint16_t light_b;


void Key_wifi_led_on(void);
void Key_wifi_led_off(void);

void Key_cc_on(void);
void Key_cc_off(void);
void Key_cc_stop(void);

void Set_pwm_light_lv(void);
void Wifi_Work_state_led(void);
uint8_t light_init(void);

#endif