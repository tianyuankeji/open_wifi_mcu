/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "user_uart.h"
#include "data_handle.h"
#include "string.h"

unsigned char volatile uart_queue_buf[UART_RECV_DATA_LEN];
unsigned char wifi_uart_rx_buf[UART_RECV_DATA_LEN];
unsigned char wifi_uart_tx_buf[UART_RECV_DATA_LEN];

volatile unsigned char *uart_queue_in;
volatile unsigned char *uart_queue_out;
volatile unsigned short queue_total_data;
/*****************************************************
                  从队列中取出数据
****************************************************/

static unsigned char uart_queue_read_byte(void){
  unsigned char value;
  
  if(queue_total_data > 0){
    if(uart_queue_out >= (unsigned char *)(uart_queue_buf + UART_RECV_DATA_LEN)){
      uart_queue_out = (unsigned char *)uart_queue_buf;
    }
    value = *uart_queue_out++;
    queue_total_data--;
  }
  return value;
}

/*****************************************************
                  串口数据存入队列
****************************************************/

static void uart_rcv_input(unsigned char value){
  if(queue_total_data < UART_RECV_DATA_LEN){
    if(uart_queue_in >= (unsigned char *)(uart_queue_buf + UART_RECV_DATA_LEN)){
      uart_queue_in = (unsigned char *)(uart_queue_buf);
    }
    *uart_queue_in++ = value;
    queue_total_data++;
  }
  else
    return ;
}


/*****************************************************
          usart3串口中断数据接收函数
****************************************************/

void USART3_IRQHandler(void){
   uint8_t ch;
   if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
     ch = (uint8_t)USART_ReceiveData(USART3);
     
     uart_rcv_input(ch);
   }
}

/*****************************************
      uart2 初始化函数
      BaudRate:设置的串口波特率
*****************************************/
uint8_t uart2_init(uint32_t BaudRate){
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  if(BaudRate > 115200){
    return (ERROR);
  }
  // PA2 PA3
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  
  USART_ClearFlag(USART2, USART_FLAG_TC);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_Cmd(USART2, ENABLE);
  
  return (SUCCESS);
}

/*****************************************
      uart3 初始化函数
      BaudRate:设置的串口波特率
*****************************************/
uint8_t uart3_init(uint32_t BaudRate){
USART_InitTypeDef USART_InitStructure; 
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  if(BaudRate > 115200) 
  {                                           
    return(ERROR);
  }

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  /*
  *  USART3_TX -> PB10 , USART13_RX -> PB11
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);		   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure); 
  //
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
  
  USART_ClearFlag(USART3,USART_FLAG_TC);
  
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                             //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                     //抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                            //指定响应优先级别
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                               //允许中断
  NVIC_Init(&NVIC_InitStructure);                                               //写入设置
  
  USART_Cmd(USART3, ENABLE);
  
  return(SUCCESS);
}

/*********************************************
         uart接收数据缓存队列初始化
*********************************************/

void uart_data_queue_init(void){
  uart_queue_in = (unsigned char *)uart_queue_buf;
  uart_queue_out = (unsigned char *)uart_queue_buf;
  queue_total_data = 0;
}

/**********************************************
          计算校验和
**********************************************/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len){
  unsigned short i;
  unsigned char check_sum = 0;
  
  for(i = 0; i < pack_len; i++){
    check_sum += *pack++;
    
  }
  return check_sum;
}

/*********************************************
            uart3发送函数
             ch：待发送字符
*********************************************/
void uart_putchar(uint8_t ch){
  USART_SendData(USART3, ch);
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}

/**********************************************
            uart3串口发送数据函数
      
**********************************************/
void uart_send_string(uint8_t *buf, uint16_t size_len){
  uint8_t *ch_tmp = buf;
  while(size_len--){
    uart_putchar((*ch_tmp));
    ch_tmp++;
  }
}

/*********************************************
            uart数据预处理函数
*********************************************/

void uart_recv_data_handle(void){
  static unsigned short rx_in = 0;
  unsigned short offset = 0;
  unsigned short rx_value_len = 0;
  while(rx_in < UART_RECV_DATA_LEN && queue_total_data > 0){
    wifi_uart_rx_buf[rx_in++] = uart_queue_read_byte();
  }
  if(rx_in < PROTOCOL_HEAD)
    return ;
  
  while((rx_in - offset) >= PROTOCOL_HEAD){
    if(wifi_uart_rx_buf[offset + HEAD_FIRST] != FRAME_FIRST){
      offset++;
      continue;
    }
    if(wifi_uart_rx_buf[offset + HEAD_SECOND] != FRAME_SECOND){
      offset++;
      continue;
    }
    
    rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] *0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW];
    if(rx_value_len > UART_RECV_DATA_LEN){
      offset += 2;
      continue;
    }
    
    if((rx_in - offset) < rx_value_len){
      break;
    }
    
    if(get_check_sum((unsigned char *)wifi_uart_rx_buf + offset, rx_value_len -1) != wifi_uart_rx_buf[offset + rx_value_len - 1]){
      offset += 2;
      continue;
    }
    data_handle(offset);
    offset += rx_value_len;
    
  }
  rx_in -= offset;
  if(rx_in > 0)
   memcpy(wifi_uart_rx_buf, wifi_uart_rx_buf + offset, rx_in);
}