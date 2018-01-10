#include "delay.h"
#include "stm32f10x.h"

static uint8_t fac_us = 0;
static uint8_t fac_ms = 0;

void delay_init(){
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
  
  fac_us = SystemCoreClock/8000000;
  fac_ms = (u16)fac_us*1000;
}

void delay_us(uint32_t nus){
  uint32_t temp;
  /*
  SysTick->LOAD = nus*fac_us;
  SysTick->VAL = 0x00;
  do{
    temp = SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1 << 16)));
  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL = 0x00;
  */
  while(nus--){
    temp = 10;
    while(temp--){
    }
  }
}

void delay_ms(u16 nms){	 		  	  
  uint32_t temp;	
  /*
  SysTick->LOAD=(u32)nms*fac_ms;
  SysTick->VAL =0x00;           
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;        
  do{
    temp=SysTick->CTRL;
  }while(temp&0x01&&!(temp&(1<<16)));
  SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;      
  SysTick->VAL =0X00;     */
  while(nms--){
    delay_us(1000);
  }
} 

