/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __GPIO_I2C_H__
#define __GPIO_I2C_H__

#include "stm32f10x.h"
#include "stdint.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0x00080000;}//上下拉输入
#define SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=0x00030000;}//通用推挽输出

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define IIC_SCL    PAout(5) //SCL输出
#define IIC_SDA    PAout(4) //SDA输出	 
#define READ_SDA   PAin(4)  //SDA输入 

#define WIFI_STATUS_LED  GPIO_Pin_1
#define INIT_SDA          GPIO_Pin_4
#define INIT_SCL          GPIO_Pin_5

#define SMART_CONFIG_START      2
#define WIFI_CONNECT_SUCCESS    0
#define WIFI_NOT_CONNECT        1

extern uint8_t send_sn;
extern uint8_t wifi_work_status;

void Key_wifi_led_on(void);
void Key_wifi_led_off(void);

void IIC_NAck(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
void IIC_Stop(void);
void IIC_Start(void);
void IIC_Init(void);

void Wifi_Work_state_led(void);
void IIC_init(void);

#endif