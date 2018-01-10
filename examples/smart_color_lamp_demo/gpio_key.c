/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_key.h"
#include "user_timer.h"
#include "string.h"
#include "gpio_rgb.h"
#include "user_uart.h"


#define KEY_MAX 3

uint8_t send_sn = 0;
uint8_t wifi_work_status = 1;

typedef void (* KEY_HANDLE)(void);

typedef struct{
  uint8_t status;
  uint32_t start_time;
  KEY_HANDLE Short_press_hanlde;
  KEY_HANDLE Long_press_handle;
}KEY_STATUS;

static KEY_STATUS Key_Status[KEY_MAX];
/****************************************************
                  入网命令发送函数
*****************************************************/
void send_wifi_connect_server(){
  wifi_work_status = 0x02;
  unsigned char send_CW[13] = {0X5C, 0XC5, 0X0D, 0X00, send_sn++, 0X11, 0X01,0X00,0X00,0X02,0XD6,0X00, 0X01};
  send_CW[12] = get_check_sum(send_CW,12);
  uart_send_string(send_CW,13);
}

/****************************************************
                按键处理消息发送函数
*****************************************************/
void mcu_send_led_status(uint8_t point, uint16_t pwm_light_lv){
  unsigned char send_buff[16] = {0x5c,0xc5,0x10,0x00,send_sn++,0x10,point,0x13,0x00,0x0a,0x4A,0x00,0x02,(uint8_t)(pwm_light_lv & 0xFF), (uint8_t)(pwm_light_lv >> 8)};
  send_buff[15] = get_check_sum(send_buff, 15);
  uart_send_string(send_buff,16);
}

/****************key1短按处理函数*******************/
static void Key1_Short_press_handle(void){
   if(light_b == 0)
     light_b = 255;
   else{
     light_b = 0;
     Key_RGB_B_off();
     
   }
   mcu_send_led_status(1, light_b);
}

/****************key1长按处理函数*******************/
static void Key1_Long_press_handle(void){
  mcu_send_led_status(1, (255 - light_b));
  if(light_b == 0){
    rgb_key_press |= (1 << 1);
  }
  else
    rgb_key_press |= (1 << 5);
  TIM_Cmd(TIM4, ENABLE);
  //send_reset_wifi();
  
}

/****************key2短按处理函数*******************/
static void Key2_Short_press_handle(void){
  if(light_r == 0)
    light_r = 255;
  else{
    light_r = 0;
    Key_RGB_R_off();
  } 
  mcu_send_led_status(2, light_r);
}

/****************key2长按处理函数*******************/
static void Key2_Long_press_handle(void){
  send_wifi_connect_server();
}

/****************key3短按处理函数*******************/
static void Key3_Short_press_handle(void){

  if(light_g == 0)
    light_g = 255;
  else{
    light_g = 0;
    Key_RGB_G_off();
  }
  mcu_send_led_status(3, light_g);
}

/****************key3长按处理函数*******************/
static void Key3_Long_press_handle(void){
   mcu_send_led_status(3, (255 - light_g));
  if(light_g == 0){
    rgb_key_press |= (1 << 2);    
  }
  else
     rgb_key_press |= (1 << 6);
  TIM_Cmd(TIM4, ENABLE);
 
}

/***************************************************
               获取按键按下持续时间
****************************************************/
static uint32_t Get_key_press_time(uint32_t start_time){
  uint32_t time;
  time = get_time_value();
  if(time >= start_time){
    time -= start_time;
  }
  else{
    time += ~start_time;
  }
  return time;
}

/***************************************************
                  获取按键状态
***************************************************/
uint8_t get_key_status(){
  uint8_t read_key_status = 0;
  if(!GPIO_ReadInputDataBit(GPIOB,GPIO_KEY1_PIN)){
    read_key_status += 1;
  }
  if(!GPIO_ReadInputDataBit(GPIOB,GPIO_KEY2_PIN)){
    read_key_status += (1 << 1);
  }
  if(!GPIO_ReadInputDataBit(GPIOB,GPIO_KEY3_PIN)){
    read_key_status += (1 << 2);
  }
  return read_key_status;
}

/**************************************************
                  按键检测函数
**************************************************/
void Key_detection(void){
  uint8_t key_tmp;
  uint32_t time;
  uint8_t key_status;
  
  key_status = get_key_status();
  
  for(key_tmp = 0; key_tmp < KEY_MAX; key_tmp++){
    switch(Key_Status[key_tmp].status){
      case KEY_SPD:
        if((key_status >> key_tmp)& 0x01){
          Key_Status[key_tmp].status = KEY_DOWN;
          Key_Status[key_tmp].start_time = get_time_value();
        }
        break;
        
      case KEY_DOWN:
        if(((key_status >> key_tmp) & 0x01) == 0){
          Key_Status[key_tmp].status = KEY_UP;
        }
        break;
      case KEY_LONG_PRESS:
        if(((key_status >> key_tmp) & 0x01) == 0){
          Key_Status[key_tmp].start_time = 0;
          Key_Status[key_tmp].status = KEY_SPD;
        }
        break;
    }
    if((Key_Status[key_tmp].status == KEY_DOWN) || (Key_Status[key_tmp].status == KEY_LONG_PRESS)){
      time = Get_key_press_time(Key_Status[key_tmp].start_time);
      if(time >= KEY_LONG_PRESS_TIME){
        Key_Status[key_tmp].start_time = get_time_value();
        Key_Status[key_tmp].status = KEY_LONG_PRESS;
        Key_Status[key_tmp].Long_press_handle();
      }
    }
    else if(Key_Status[key_tmp].status == KEY_UP){
      time = Get_key_press_time(Key_Status[key_tmp].start_time);
      Key_Status[key_tmp].start_time = 0;
      Key_Status[key_tmp].status = KEY_SPD;
      if((time >= KEY_SHORT_PRESS_TIME) && (time < KEY_LONG_PRESS_TIME)){
        Key_Status[key_tmp].Short_press_hanlde();
      }
      else if(time >= KEY_LONG_PRESS_TIME){
        Key_Status[key_tmp].Long_press_handle();
      }
    }
  }
}

/**************************************************
                按键初始化函数
**************************************************/
uint8_t Key_init(void){
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
  
  GPIO_InitStruct.GPIO_Pin = GPIO_KEY1_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_KEY2_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_KEY3_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  memset(Key_Status, 0, sizeof(Key_Status));
  
  Key_Status[0].Short_press_hanlde = Key1_Short_press_handle;
  Key_Status[0].Long_press_handle = Key1_Long_press_handle;
  
  Key_Status[1].Short_press_hanlde = Key2_Short_press_handle;
  Key_Status[1].Long_press_handle = Key2_Long_press_handle;
  
  Key_Status[2].Short_press_hanlde = Key3_Short_press_handle;
  Key_Status[2].Long_press_handle = Key3_Long_press_handle;
  
  return (SUCCESS);
}