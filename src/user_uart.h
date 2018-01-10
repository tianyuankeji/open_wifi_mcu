/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#ifndef __USER_UART_H__
#define __USER_UART_H__

#include "stm32f10x.h"
#include "stdint.h"

/*******************************************************
                  Êý¾ÝÖ¡×Ö½ÚÐò
*******************************************************/
#define HEAD_FIRST      0
#define HEAD_SECOND     1
#define LENGTH_LOW      2
#define LENGTH_HIGH     3
#define DEVICE_SN       4
#define FEAME_CONTROL   5 
#define POINT           6
#define CLUSTER         7
#define FRAME_TYPE      9
#define DATA_START      10

#define PROTOCOL_HEAD   13
#define FRAME_FIRST     0x5C
#define FRAME_SECOND    0xC5



#define UART_RECV_DATA_LEN      256

extern unsigned char volatile uart_queue_buf[UART_RECV_DATA_LEN];
extern unsigned char wifi_uart_rx_buf[UART_RECV_DATA_LEN];
extern unsigned char wifi_uart_tx_buf[UART_RECV_DATA_LEN];

extern volatile unsigned char *uart_queue_in;
extern volatile unsigned char *uart_queue_out;
extern volatile unsigned short queue_total_data;


uint8_t uart2_init(uint32_t BaudRate);

uint8_t uart3_init(uint32_t BaudRate);

void uart_data_queue_init(void);

unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);

void uart_putChar(uint8_t ch);

void uart_send_string(uint8_t *buf, uint16_t size_len);

void uart_recv_data_handle(void);

#endif