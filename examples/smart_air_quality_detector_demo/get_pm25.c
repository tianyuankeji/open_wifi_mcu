#include "get_pm25.h"
#include "user_uart.h"
#include "string.h"
#include "gpio_key.h"

#define UART2_BUFF_SIZE 128

unsigned char volatile uart2_buf[UART2_BUFF_SIZE];
unsigned char uart2_queue_buf[UART2_BUFF_SIZE];

uint16_t Pm25_value = 0;
uint16_t Pm_25_tmp[20];
volatile unsigned char *uart2_queue_in;
volatile unsigned char *uart2_queue_out;
volatile unsigned short queue2_total_data;
unsigned char Tim4_value = 0;

/*****************************************************
          usart2 队列数据读取函数
****************************************************/
static unsigned char uart2_queue_read_byte(void){
  unsigned char value;
  if(queue2_total_data > 0){
    if(uart2_queue_out >= (unsigned char *)(uart2_queue_buf + UART2_BUFF_SIZE)){
      uart2_queue_out = (unsigned char *)uart2_queue_buf;
    }
    value = *uart2_queue_out++;
    queue2_total_data--;
  }
  return value;
}
/*****************************************************
          usart2 队列数据存储函数
****************************************************/
static void uart2_queue_rcv_input(unsigned char value){
  if(queue2_total_data < UART2_BUFF_SIZE){
    if(uart2_queue_in >= (unsigned char *)(uart2_queue_buf + UART2_BUFF_SIZE)){
      uart2_queue_in = (unsigned char *)(uart2_queue_buf);
    }
    *uart2_queue_in++ = value;
    queue2_total_data++;
  }
  else
    USART_Cmd(USART2, DISABLE);
}

/*****************************************************
          usart2 接收数据缓存队列初始化
****************************************************/
void uart2_data_queue_init(void){
  uart2_queue_in = (unsigned char *)uart2_queue_buf;
  uart2_queue_out = (unsigned char *)uart2_queue_buf;
  queue2_total_data = 0;
}

/*****************************************************
          usart2 串口中断数据接收函数
****************************************************/
void USART2_IRQHandler(void){
  uint8_t ch;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
   
   ch = (uint8_t)USART_ReceiveData(USART2);
   uart2_queue_rcv_input(ch);
  }
}


/*****************************************************
              Pm2.5数值获取对比函数
****************************************************/
static uint16_t ValueConvert(uint16_t adc_voltage_value){
  float voltage = (float)adc_voltage_value * 5.0 / 1024.0;
  if(voltage < 0.040){
    return (uint16_t)(voltage * 200 + 8 + 0.5);
  }
  if( voltage < 0.048){
    return ( uint16_t)(voltage * 400);
  }
  if( voltage < 0.051){
   return ( uint16_t)(voltage * 600);
  }
  if( voltage < 0.054){
   return ( uint16_t)(voltage * 750);
  }
  if( voltage < 0.058){
    return ( uint16_t)(voltage * 900);
  }
  if( voltage < 0.064){
    return ( uint16_t)(voltage * 1000);
  } 
  if( voltage < 0.070){
    return ( uint16_t)(voltage * 1250);
  }	
  if( voltage < 0.075){
    return ( uint16_t)(voltage * 1400);
  }	 
  if( voltage < 0.080){
    return ( uint16_t)(voltage * 1700);
  } 
  if( voltage < 0.085){
    return ( uint16_t)(voltage * 1800);
  } 
  if( voltage < 0.090){
    return ( uint16_t)(voltage * 1900);
  } 
  if( voltage < 0.100){
    return ( uint16_t)(voltage * 2000);
  }
  if( voltage < 0.110){
    return ( uint16_t)(voltage * 2200);
  }	 
  return ( uint16_t)(voltage * 3000);
}

/*****************************************************
              uart2数据处理函数
****************************************************/
void uart2_recv_data_handle(void){
  static unsigned short rx_in = 0;
  unsigned short offset = 0;
  unsigned short rx_value_len = 0;
  unsigned char recv_number = 0;
  uart_send_string("11", 2);
  while(rx_in < UART2_BUFF_SIZE && queue2_total_data){
    uart2_buf[rx_in++] = uart2_queue_read_byte();
  }
  if(rx_in < 7){
    return ;
  }
  uart_send_string("22", 2);
  while(offset <= (rx_in - 7)){
    if(uart2_buf[offset] != 0xaa){
      offset++;
      continue;
    }
    if(uart2_buf[offset + 6] != 0xff){
      offset++;
      continue;
    }
    if((uart2_buf[offset + 1] + uart2_buf[offset + 2] + uart2_buf[offset + 3] + uart2_buf[offset + 4]) != uart2_buf[offset + 5]){
      offset++;
      continue;
    }
    Pm_25_tmp[recv_number] = ValueConvert((uint16_t)uart2_buf[offset + 1] * 256 + uart2_buf[offset + 2]);
    recv_number++;
    offset += 7;
  } 
  memset((void *)uart2_buf, 0, UART2_BUFF_SIZE);
  rx_in = 0;
  uint32_t CalcTemp = 0;
  rx_value_len = recv_number;
  while(recv_number--){
    CalcTemp += Pm_25_tmp[recv_number];
  }
  Pm25_value = CalcTemp / rx_value_len;
  memset((void *)Pm_25_tmp, 0, sizeof(int16_t) * 20);
  uint8_t tmp[2];
  tmp[1] = (Pm25_value >> 8);
  tmp[0] = Pm25_value & 0xFF;
  mcu_send_led_status(0x04C0, 0x0000, 0x01, 2, tmp);
}

/*****************************************************
              Timer4定时器处理函数
****************************************************/
void TIM4_IRQHandler(void){
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    
    uart2_recv_data_handle();
    USART_Cmd(USART2, ENABLE);
    
  }
}


/*****************************************************
                PM2.5初始化函数
****************************************************/

void Pm25_init(void){
  uart2_init(2400);
  uart2_data_queue_init();
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);                          //时钟使能
  TIM_DeInit(TIM4);
  
  //50us * 200 * 500  = 5s
   
  //定时器TIM3初始化
  TIM_TimeBaseStructure.TIM_Period = 7199;                                      	
  TIM_TimeBaseStructure.TIM_Prescaler = 49999;                                    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                       
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                   
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                               

  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                                     

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;                               
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;                            
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               
  NVIC_Init(&NVIC_InitStructure);                                                

  TIM_Cmd(TIM4, ENABLE);
}