/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_light.h"
#include "gpio_key.h"
#include "user_timer.h"

LIGHT_MOD Light_mod[3] = {{GPIOB, GPIO_Pin_5}, {GPIOB, GPIO_Pin_6}, {GPIOA, GPIO_Pin_0}};

/******************************************************
                wifi指示灯状态函数
******************************************************/
void Key_wifi_led_on(void){
  GPIO_ResetBits(GPIOA, WIFI_STATUS_LED);
}
void Key_wifi_led_off(void){
  GPIO_SetBits(GPIOA, WIFI_STATUS_LED);
}

/******************************************************
                key1指示灯状态函数
******************************************************/
void Key_led1_on(void){
  GPIO_ResetBits(GPIOB, KEY_LED1);
}
void Key_led1_off(void){
  GPIO_SetBits(GPIOB, KEY_LED1);
}

/******************************************************
                key2指示灯状态函数
******************************************************/
void Key_led2_on(void){
  GPIO_ResetBits(GPIOB, KEY_LED2);
}
void Key_led2_off(void){
  GPIO_SetBits(GPIOB, KEY_LED2);
}

/******************************************************
                key3指示灯状态函数
******************************************************/

void Key_led3_on(void){
  GPIO_ResetBits(GPIOA, KEY_LED3);
}
void Key_led3_off(void){
  GPIO_SetBits(GPIOA, KEY_LED3);
}


/******************************************************
                wifi状态指示函数
******************************************************/
void Wifi_Work_state_led(void){
  switch(wifi_work_status){
    case SMART_CONFIG_START:
      if(get_time2_value() % 2)
        Key_wifi_led_on();
      else
        Key_wifi_led_off();
    break;
    case WIFI_CONNECT_SUCCESS:
      Key_wifi_led_on();
    break;
    case WIFI_NOT_CONNECT:
      Key_wifi_led_off();
    break;
  }
}

/******************************************************
                Ledlight 初始化函数
******************************************************/

uint8_t Light_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = WIFI_STATUS_LED | KEY_LED3;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = KEY_LED1 | KEY_LED2 | WIFI_RST;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOB, WIFI_RST);
  
  return (SUCCESS);
}