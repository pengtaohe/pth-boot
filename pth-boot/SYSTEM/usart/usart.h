#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define USART_REC_LEN  			960*1024 /*app store in 1024k inner flash, reserve 64k inner flash for pth-boot  ,hept, 2017.9.7 */
	  	
extern u8  USART_RX_BUF[USART_REC_LEN];
extern u32 USART_RX_CNT;	

void uart_init(u32 bound);
void uart_interrupt_init(u8 enable);
#endif


