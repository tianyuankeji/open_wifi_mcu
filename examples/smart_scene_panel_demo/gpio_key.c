/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_key.h"
#include "user_timer.h"
#include "string.h"
#include "gpio_light.h"
#include "user_uart.h"
#include "data_handle.h"

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
void mcu_send_led_status(uint16_t cluster, uint16_t attribute, uint8_t Point,uint8_t data_len,uint8_t* value){
  unsigned char send_buff[50];
  send_buff[HEAD_FIRST] = 0x5c;
  send_buff[HEAD_SECOND] = 0xc5;
  send_buff[DEVICE_SN] = send_sn++;
  send_buff[FEAME_CONTROL] = 0x10;      // 上行 有应答
  send_buff[POINT] = Point;
  send_buff[CLUSTER] = (uint8_t)(cluster & 0xff);
  send_buff[CLUSTER + 1] = (uint8_t)((cluster >> 8) & 0xff);
  send_buff[FRAME_TYPE] = REPORT_ORDER;
  send_buff[DATA_START] = (uint8_t)(attribute & 0xff);
  send_buff[DATA_START + 1] = (uint8_t)((attribute >> 8) & 0xff);
  send_buff[DATA_START + 2] = data_len;
  memcpy(send_buff + DATA_START + 3, value, send_buff[DATA_START + 2]);
  send_buff[LENGTH_LOW] = (uint8_t)((PROTOCOL_HEAD + send_buff[DATA_START + 2] + 1) & 0xff);
  send_buff[LENGTH_HIGH] = (uint8_t)(((PROTOCOL_HEAD + send_buff[DATA_START + 2] + 1) >> 8) & 0xff);
  send_buff[PROTOCOL_HEAD + send_buff[DATA_START + 2]] = get_check_sum(send_buff, (PROTOCOL_HEAD + send_buff[DATA_START + 2]));
  uart_send_string(send_buff,(PROTOCOL_HEAD + send_buff[DATA_START + 2] + 1));
}

/****************key1短按处理函数*******************/
static void Key1_Short_press_handle(void){
  uint8_t value = 1;
  Key_get_home();
  mcu_send_led_status(0x0005, 0x00C0, 0x01, 1, &value);
}

/****************key1长按处理函数*******************/
static void Key1_Long_press_handle(void){
   
}

/****************key2短按处理函数*******************/
static void Key2_Short_press_handle(void){
  uint8_t value = 2;
  Key_leave_home();
  mcu_send_led_status(0x0005, 0x00C0, 0x01, 1, &value);
}

/****************key2长按处理函数*******************/
static void Key2_Long_press_handle(void){
  send_wifi_connect_server();
}

/****************key3短按处理函数*******************/
static void Key3_Short_press_handle(void){
  uint8_t value = 0x03;
  Key_sleep_mode();
  mcu_send_led_status(0x0005, 0x00C0, 0x01, 1, &value);
}

/****************key3长按处理函数*******************/
static void Key3_Long_press_handle(void){
   

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