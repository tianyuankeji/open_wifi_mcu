/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/

#include "user_timer.h"
#include "include.h"
#include "user_uart.h"
uint32_t Timer2_value = 0;
uint32_t Timer_count_value = 0;


/**************************************************************
                获取计时时间  500ms
**************************************************************/
uint32_t get_time2_value(void){
  return Timer2_value;
}

/**************************************************************
                获取计时时间  ms
**************************************************************/

uint32_t get_time_value(void){
  return Timer_count_value;
}

/**************************************************************
        timer2定时器       中断处理函数
**************************************************************/
void TIM2_IRQHandler(void){
  
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Timer_count_value++;
    if(Timer_count_value % 500 == 0){
      Timer2_value++;
      if(Timer2_value == 12)
        Timer2_value = 0;
    }
  }
}

/**************************************************************
        初始化timer2定时器 用于计算时间
**************************************************************/

uint8_t Timer2_init(void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);                          //时钟使能
  TIM_DeInit(TIM2);
  
  //1ms
   
  //定时器TIM2初始化
  TIM_TimeBaseStructure.TIM_Period = 719;                                       	
  TIM_TimeBaseStructure.TIM_Prescaler = 99;                                    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                       
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                   
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                               

  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                                     

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;                               
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;                            
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               
  NVIC_Init(&NVIC_InitStructure);                                                
  
  Timer2_value = 0;

  TIM_Cmd(TIM2, ENABLE);
  
  return (SUCCESS);
}