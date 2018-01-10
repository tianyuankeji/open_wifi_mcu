/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "gpio_light.h"
#include "gpio_key.h"
#include "user_timer.h"

uint32_t Timer3_value = 0;
uint16_t light_r, light_g, light_b;

uint8_t key_press = 0;

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
                b指示灯状态函数
******************************************************/
void Key_RGB_B_on(void){
  GPIO_ResetBits(GPIOA, KEY_RGB_B);
}
void Key_RGB_B_off(void){
  GPIO_SetBits(GPIOA, KEY_RGB_B);
}

/******************************************************
               r指示灯状态函数
******************************************************/
void Key_RGB_R_on(void){
  GPIO_ResetBits(GPIOA, KEY_RGB_R);
}
void Key_RGB_R_off(void){
  GPIO_SetBits(GPIOA, KEY_RGB_R);
}

/******************************************************
               g指示灯状态函数
******************************************************/

void Key_RGB_G_on(void){
  GPIO_ResetBits(GPIOA, KEY_RGB_G);
}
void Key_RGB_G_off(void){
  GPIO_SetBits(GPIOA, KEY_RGB_G);
}


/******************************************************
                情景模式状态执行函数
******************************************************/
void Key_get_home(void){
  GPIO_ResetBits(GPIOB, KEY_LIGHT_1);
  GPIO_SetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOA, KEY_LIGHT_3);
  light_r = 255;
  light_g = 255;
  light_b = 255;
  
}
void Key_leave_home(void){
  GPIO_ResetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  GPIO_SetBits(GPIOA, KEY_LIGHT_3);
  light_r = 0;
  light_g = 0;
  light_b = 0;
  Set_pwm_light_lv();
}
void Key_sleep_mode(void){
  GPIO_ResetBits(GPIOA, KEY_LIGHT_3);
  GPIO_SetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  light_r = 30;
  light_g = 100;
  light_b = 30;
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
                pwm状态处理指示函数
******************************************************/
void Set_pwm_light_lv(void){
  if(light_r == 0){
    Key_RGB_R_off();
  }
  if(light_g == 0){
    Key_RGB_G_off();
  }
  if(light_b == 0){
    Key_RGB_B_off();
  }
}


/******************************************************
                PWM控制函数
******************************************************/
void RGB_tmp_value(){
  if(light_r > 0){
    if(Timer3_value == light_r){
        Key_RGB_R_off();
     }
  }
  if(light_g > 0){
    if(Timer3_value == light_g){
        Key_RGB_G_off();
     }
  }
  if(light_b > 0){
    if(Timer3_value == light_b){
        Key_RGB_B_off();
     }
  }
  if(Timer3_value >= 255){
    Timer3_value = 0;
    if(light_r > 0){
      Key_RGB_R_on();
    }
    if(light_g > 0){
      Key_RGB_G_on();
    }
    if(light_b > 0){
      Key_RGB_B_on();
    }
  }
}

/******************************************************
                Timer3中断处理函数
******************************************************/
void TIM3_IRQHandler(void){
if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    Timer3_value++;
    RGB_tmp_value();
  }
}


/******************************************************
                RGBlight 初始化函数
******************************************************/

uint8_t light_init(void){
  
  light_b = 0;
  light_r = 0;
  light_g = 0;
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = WIFI_STATUS_LED | KEY_RGB_B | KEY_RGB_R | KEY_RGB_G | KEY_LIGHT_3;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = KEY_LIGHT_1 | KEY_LIGHT_2;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOA, KEY_LIGHT_3);
  GPIO_ResetBits(GPIOB, KEY_LIGHT_2);
  GPIO_SetBits(GPIOB, KEY_LIGHT_1);
  
  Set_pwm_light_lv();
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);                          //时钟使能
  TIM_DeInit(TIM3);
  
  //50us
   
  //定时器TIM3初始化
  TIM_TimeBaseStructure.TIM_Period = 35;                                      	
  TIM_TimeBaseStructure.TIM_Prescaler = 99;                                    
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
  
  Timer3_value = 0;

  TIM_Cmd(TIM3, ENABLE);
  
  // 25ms
  
  return (SUCCESS);
}