/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_I2C.h"
#include "user_timer.h"
#include "delay.h"

uint8_t wifi_work_status = WIFI_NOT_CONNECT;

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
                IIC停止信号产生函数
******************************************************/
void IIC_Stop(void){
  SDA_OUT();
  IIC_SCL = 0;
  IIC_SDA = 0;
  delay_us(14);
  IIC_SCL = 1;
  delay_us(14);
  IIC_SDA = 1;
}

/******************************************************
                IIC起始信号产生函数
******************************************************/

void IIC_Start(void){
  SDA_OUT();
  IIC_SDA = 1;
  IIC_SCL = 1;
  delay_us(14);
  IIC_SDA = 0;
  delay_us(14);
  IIC_SCL = 0;
}


/******************************************************
                IIC产生非ACK应答函数
******************************************************/
void IIC_NAck(void){
  IIC_SCL = 0;
  SDA_OUT();
  IIC_SDA = 1;
  delay_us(15);
  IIC_SCL = 1; 
  delay_us(15);
  IIC_SCL = 0;
}


/******************************************************
                IIC产生ACK应答函数
******************************************************/
void IIC_Ack(void){
  IIC_SCL = 0;
  SDA_OUT();
  IIC_SDA = 0;
  delay_us(15);
  IIC_SCL = 1;
  delay_us(15);
  IIC_SCL = 0;
}


/******************************************************
                IIC接收应答函数
                1：接收应答失败
                0：接收应答成功
******************************************************/
uint8_t IIC_Wait_Ack(void){
  uint8_t ucErrTime = 0;
  IIC_SCL = 0;
  IIC_SDA = 1;
  SDA_IN();
  delay_us(14);
  IIC_SCL = 1;
  delay_us(14);
  while(READ_SDA){
    ucErrTime++;
    if(ucErrTime > 200){
      IIC_Stop();
      return 1;
    }
    delay_us(1);
  }
  IIC_SCL = 0;
  delay_us(10);
  return 0;
}


/******************************************************
                IIC接收一个字节函数
            ack = 1 发送ack ack = 0 发送nack
******************************************************/
uint8_t IIC_Read_Byte(unsigned char ack){
  unsigned char i, receive = 0;
  SDA_IN();
  for(i = 0; i < 8; i++){
    IIC_SCL = 0;
    delay_us(50);
    IIC_SCL = 1;
    delay_us(50);
    receive <<= 1;
    if(READ_SDA)
      receive++;
  }
  IIC_SCL = 0;
  if(!ack)
    IIC_NAck();
  else
    IIC_Ack();
  return receive;
}


/******************************************************
                IIC发送一个字节函数
******************************************************/
void IIC_Send_Byte(uint8_t txd){
  uint8_t t;
  SDA_OUT();
  IIC_SCL = 0;
  for(t = 0; t < 8; t++){
    IIC_SCL = 0;
    if((txd & 0x80) >> 7)
      IIC_SDA = 1;
    else
      IIC_SDA = 0;
    txd <<= 1;
    delay_us(15);
    IIC_SCL = 1;
    delay_us(15);
  }
  IIC_SCL = 0;
}




/******************************************************
                RGBlight 初始化函数
******************************************************/

void IIC_init(void){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = WIFI_STATUS_LED;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStruct.GPIO_Pin = INIT_SDA | INIT_SCL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  
}