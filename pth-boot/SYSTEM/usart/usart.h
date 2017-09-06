#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART_REC_LEN  			120*1024 //定义最大接收字节数 120K
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];
extern u32 USART_RX_CNT;	

void uart_init(u32 bound);
void uart_interrupt_init(u8 enable);
#endif


