#include "dht12.h"
#include "gpio_I2C.h"
#include "delay.h"
#include "stm32f10x.h"
#include "user_uart.h"
#include "data_handle.h"
#include "user_uart.h"
#include "string.h"

float Temprature, Humi;
uint8_t send_sn = 0;

/****************************************************
                消息发送函数
*****************************************************/
void mcu_send_data_status(uint16_t cluster, uint16_t attribute, uint8_t Point,uint8_t data_len,uint8_t* value){
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

/******************************************************
                DHT12温湿度读取函数
******************************************************/
void sensor_read(void){
  uint16_t i;
  uint8_t Humi_H, Humi_L, Temp_H, Temp_L, Temp_CAL, temp;
  
  IIC_Start();
  IIC_Send_Byte(0xB8);
  if(!IIC_Wait_Ack()){
    i = 0;
    IIC_Send_Byte(0);
    while(IIC_Wait_Ack()){
      if(++i >= 500)
        break;
    }
    i = 0;
    IIC_Start();
    IIC_Send_Byte(0xB9);
    while(IIC_Wait_Ack()){
      if(++i >= 500)
        break;
    }
    Humi_H = IIC_Read_Byte(1);
    Humi_L = IIC_Read_Byte(1);
    Temp_H = IIC_Read_Byte(1);
    Temp_L = IIC_Read_Byte(1);
    Temp_CAL = IIC_Read_Byte(0);
    IIC_Stop();
    temp = (uint8_t)(Humi_H + Humi_L + Temp_H + Temp_L);
    uint8_t buf[2];
    if(Temp_CAL == temp){
      buf[0] = (((Humi_H * 100) + Humi_L * 10) & 0xFF);
      buf[1] = (((Humi_H * 100) + Humi_L * 10) >> 8);
      
      mcu_send_data_status(0x0405, 0x0000, 0x01, 2, buf);
      if(Temp_L & 0x80){
        buf[0] = (0 - (Temp_H * 10 + ((Temp_L & 0x7F))))*10 % 256;
        buf[1] = (0 - (Temp_H * 10 + ((Temp_L & 0x7F))))*10 / 256;
      }
      else{
        buf[0] = (((Temp_H * 100) + (Temp_L * 10)) & 0xFF);
        buf[1] = (((Temp_H * 100) + (Temp_L * 10)) >> 8);
      }
    }
   
    mcu_send_data_status(0x0402, 0x0000, 0x01, 2, buf);
    
  }
}


/******************************************************
                Timer3中断处理函数
******************************************************/
void TIM3_IRQHandler(void){
if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    sensor_read();
  }
}


/******************************************************
                初始化DHT12 函数
******************************************************/
void DHT12_iic_init(void){
  IIC_init();
  delay_init();
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                          //时钟使能
  TIM_DeInit(TIM3);
  
  //50us * 200 * 100  = 1s
   
  //定时器TIM3初始化
  TIM_TimeBaseStructure.TIM_Period = 7199;                                      	
  TIM_TimeBaseStructure.TIM_Prescaler = 29999;                                    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                       
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                   
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                               

  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                                     

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                               
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;                            
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               
  NVIC_Init(&NVIC_InitStructure);                                                

  TIM_Cmd(TIM3, ENABLE);
}