/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __GPIO_RGB_H__
#define __GPIO_RGB_H__

#include "stm32f10x.h"
#include "stdint.h"

#define WIFI_STATUS_LED  GPIO_Pin_1
#define KEY_RGB_B        GPIO_Pin_6
#define KEY_RGB_R        GPIO_Pin_7
#define KEY_RGB_G        GPIO_Pin_8

#define SMART_CONFIG_START      2
#define WIFI_CONNECT_SUCCESS    0
#define WIFI_NOT_CONNECT        1

typedef struct{
  uint16_t *light_value;
}PWM_MOD;

extern uint8_t rgb_key_press;
extern uint16_t light_r,light_g,light_b;
extern PWM_MOD pwm_tmp_value[3];

void Key_wifi_led_on(void);
void Key_wifi_led_off(void);

void Key_RGB_B_on(void);
void Key_RGB_B_off(void);

void Key_RGB_R__on(void);
void Key_RGB_R_off(void);

void Key_RGB_G_on(void);
void Key_RGB_G_off(void);

void Set_pwm_light_lv(void);
void Wifi_Work_state_led(void);
uint8_t RGB_init(void);

#endif