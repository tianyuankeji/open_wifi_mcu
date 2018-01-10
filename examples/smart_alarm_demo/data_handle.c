/*******************************************************
                tianyuan technology
                  2017-12-08 Frd
                      yang
*******************************************************/
#include "data_handle.h"
#include "user_uart.h"
#include "gpio_rgb.h"
#include "string.h"
#include "gpio_key.h"
#include "include.h"

/*******************************************************
                    数据协议拼接                   
*******************************************************/
void Send_Ack_Data(uint8_t *data, uint8_t data_len, uint8_t send_order){
  if(send_order == READ_ACK){
    wifi_uart_tx_buf[LENGTH_LOW] = ((PROTOCOL_HEAD + data_len + 2) & 0xFF);
    wifi_uart_tx_buf[LENGTH_HIGH] = ((PROTOCOL_HEAD + data_len + 2) >> 8);
    wifi_uart_tx_buf[FEAME_CONTROL] = (wifi_uart_tx_buf[FEAME_CONTROL]^0x18);
    wifi_uart_tx_buf[FRAME_TYPE] = send_order;
    wifi_uart_tx_buf[DATA_START + 2] = (!SUCCESS);
    wifi_uart_tx_buf[DATA_START + 3] = data_len;
    memcpy(wifi_uart_tx_buf + DATA_START + 4, data, data_len);
    wifi_uart_tx_buf[DATA_START + 4 + data_len] = get_check_sum(wifi_uart_tx_buf, (PROTOCOL_HEAD + data_len + 1));
  }
  if(send_order == WRITE_ACK){
    uint8_t buff_tmp = wifi_uart_tx_buf[DATA_START];
    wifi_uart_tx_buf[LENGTH_LOW] = ((PROTOCOL_HEAD + data_len) & 0xFF);
    wifi_uart_tx_buf[LENGTH_HIGH] = ((PROTOCOL_HEAD + data_len) >> 8);
    wifi_uart_tx_buf[FEAME_CONTROL] = (wifi_uart_tx_buf[FEAME_CONTROL]^0x18);
    wifi_uart_tx_buf[FRAME_TYPE] = send_order;
    wifi_uart_tx_buf[DATA_START] = (!SUCCESS);
    wifi_uart_tx_buf[DATA_START + 2] = wifi_uart_tx_buf[DATA_START + 1];
    wifi_uart_tx_buf[DATA_START + 1] = buff_tmp;
    wifi_uart_tx_buf[DATA_START + 3] = get_check_sum(wifi_uart_tx_buf, PROTOCOL_HEAD);
  }
  uart_send_string(wifi_uart_tx_buf, ((wifi_uart_tx_buf[LENGTH_HIGH] << 8)|(wifi_uart_tx_buf[LENGTH_LOW])));
}

/*******************************************************
                    上报数据函数状态
*******************************************************/
void report_device_ACK_status(unsigned short offset){
  if((wifi_uart_rx_buf[offset + CLUSTER] | (wifi_uart_rx_buf[offset + CLUSTER + 1] << 8)) == DEVICE_INFO){
    if(wifi_uart_rx_buf[offset + DATA_START] == DVC_CON_WIFI)
      wifi_work_status = wifi_uart_rx_buf[offset + DATA_START + 3];
  }
}

/*******************************************************
                    写数据应答函数状态
*******************************************************/
void write_device_ACK_status(unsigned short offset){
  if((wifi_uart_rx_buf[offset + CLUSTER] | (wifi_uart_rx_buf[offset + CLUSTER + 1] << 8)) == DEVICE_INFO){
    if(wifi_uart_rx_buf[offset + DATA_START] == DVC_CON_WIFI)
      wifi_work_status = SMART_CONFIG_START;
  }
}

/*******************************************************
                    写入数据函数状态
*******************************************************/
void write_device_cluster_info(unsigned short offset){
  switch((wifi_uart_rx_buf[offset + CLUSTER] | (wifi_uart_rx_buf[offset + CLUSTER + 1] << 8))){
    
    default:
      break;
  }
}
                   
/*******************************************************
                    读取数据函数状态
*******************************************************/
void read_device_cluster_info(unsigned short offset){
  switch((wifi_uart_rx_buf[offset + CLUSTER] | (wifi_uart_rx_buf[offset + CLUSTER + 1] << 8))){
    case DEVICE_INFO:
      switch((wifi_uart_rx_buf[offset + DATA_START] | (wifi_uart_rx_buf[offset + DATA_START + 1] << 8))){
        case DVC_CHIP_ID:
          memset(wifi_uart_tx_buf, 0, UART_RECV_DATA_LEN);
          memcpy(wifi_uart_tx_buf, (wifi_uart_rx_buf + offset), PROTOCOL_HEAD);
          Send_Ack_Data(CHIP_ID, strlen(CHIP_ID), READ_ACK);
        break;
        case DVC_VCODE:
          memset(wifi_uart_tx_buf, 0, UART_RECV_DATA_LEN);
          memcpy(wifi_uart_tx_buf, (wifi_uart_rx_buf + offset), PROTOCOL_HEAD);
          Send_Ack_Data(VCODE, strlen(VCODE), READ_ACK);
        break;
        case DVC_PCODE:
          memset(wifi_uart_tx_buf, 0, UART_RECV_DATA_LEN);
          memcpy(wifi_uart_tx_buf, (wifi_uart_rx_buf + offset), PROTOCOL_HEAD);
          Send_Ack_Data(PCODE, strlen(PCODE), READ_ACK);
        break;
        case DVC_HWVSION:
          memset(wifi_uart_tx_buf, 0, UART_RECV_DATA_LEN);
          memcpy(wifi_uart_tx_buf, (wifi_uart_rx_buf + offset), PROTOCOL_HEAD);
          Send_Ack_Data(HW_VERSION, strlen(HW_VERSION), READ_ACK);
        break;
        case DVC_SWVSION:
          memset(wifi_uart_tx_buf, 0, UART_RECV_DATA_LEN);
          memcpy(wifi_uart_tx_buf, (wifi_uart_rx_buf + offset), PROTOCOL_HEAD);
          Send_Ack_Data(SW_VERSION, strlen(SW_VERSION), READ_ACK);
        break;
        default:
        break;
      }
    break;
    
    default:
    break;
  }
}

/*******************************************************
                    数据分类函数
               offset：数据指针起始点
*******************************************************/
void data_handle(unsigned short offset){
  
  unsigned char cmd_type = wifi_uart_rx_buf[offset + FRAME_TYPE];
  
  switch(cmd_type){
    case READ_ORDER:
      read_device_cluster_info(offset);
    break;
    case READ_ACK:
      //read_device_ACK_status(offset);
    break;
    case WRITE_ORDER:
      write_device_cluster_info(offset);
    break;
    case WRITE_ACK:
      write_device_ACK_status(offset);
    break;
    case REPORT_ORDER:
      report_device_ACK_status(offset);
    break;
    default:
    break;
  }
}