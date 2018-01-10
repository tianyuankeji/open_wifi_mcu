#ifndef __DELAY_H__
#define __DELAY_H__

#include "stdint.h"

void delay_init(void);
void delay_ms(uint16_t nms);
extern void delay_us(uint32_t nus);

#endif