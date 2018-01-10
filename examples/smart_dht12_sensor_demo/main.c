/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/

#include "include.h"
#include "user_uart.h"
#include "user_timer.h"
#include "data_handle.h"
#include "dht12.h"
#include "gpio_I2C.h"
#include "delay.h"
#include "gpio_key.h"

void Rcc_Configuration(void){
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void Nvic_Configuration(void){
  
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
}

void Device_Init(void){
  // USART2_init
  //uart2_init(115200);
  
  // USART3_init
  uart3_init(115200);
  
  // USART_QUEUE_init
  uart_data_queue_init();
  
  // TIM2_init
  Timer2_init();

  Key_init();
  
  DHT12_iic_init();
  
}

void main(void){
  
  Rcc_Configuration();  // ±÷”≈‰÷√
  
  Nvic_Configuration(); //NVIC≈‰÷√
  
  Device_Init();
  
  while(1){    
    Key_detection();
    
    uart_recv_data_handle();
    
    Wifi_Work_state_led();
   
  }
}