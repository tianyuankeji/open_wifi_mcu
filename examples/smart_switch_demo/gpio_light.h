/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __GPIO_LIGHT_H__
#define __GPIO_LIGHT_H__

#include "stm32f10x.h"
#include "stdint.h"

#define WIFI_STATUS_LED GPIO_Pin_1
#define KEY_LED3        GPIO_Pin_0
#define WIFI_RST        GPIO_Pin_15

#define KEY_LED1        GPIO_Pin_5
#define KEY_LED2        GPIO_Pin_6


#define SMART_CONFIG_START      2
#define WIFI_CONNECT_SUCCESS    0
#define WIFI_NOT_CONNECT        1

typedef struct {
  GPIO_TypeDef *LED_PORT;
  uint16_t LED_Pin;
}LIGHT_MOD;

extern LIGHT_MOD Light_mod[3];

void Key_wifi_led_on(void);
void Key_wifi_led_off(void);

void Key_led1_on(void);
void Key_led1_off(void);

void Key_led2_on(void);
void Key_led2_off(void);

void Key_led3_on(void);
void Key_led3_off(void);

void Wifi_Work_state_led(void);
uint8_t Light_init(void);

#endif