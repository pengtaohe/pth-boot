#include "sys.h"
#include "delay.h"  
#include "usart.h"     
#include "stmflash.h" 
#include "iap.h"  
#include "shell.h"

void load_app(u8 type)  /* 1: sram,  2: flash*/
{
	u16 oldcount=0;
	u16 applenth=0;

	printf(" start receive app, please don't typewrite!\r\n");
	uart_interrupt_init(1);

	while(1)
	{
		if(USART_RX_CNT)
		{
			if(oldcount == USART_RX_CNT)
			{
				applenth = USART_RX_CNT;
				oldcount = 0;
				USART_RX_CNT = 0;
				printf(" app receive ok!\r\n");
				printf(" receive %dBytes\r\n",applenth);
				break;
			}
			else
			{
				oldcount=USART_RX_CNT;	
			}
		}

		delay_ms(10);
	}
	
	uart_interrupt_init(0);

	if(2 == type)
	{
		if(applenth)
		{	
			if(((*(vu32*)(0X20001000+4))&0xFF000000) == 0x08000000)//判断是否为0X08XXXXXX.
			{	
				printf(" write flash app to flash...");
				iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);
				printf("\b\b\b ok!\r\n");	
			}
			else 
			{	   
				printf("received file is not flash app.\r\n");
			}
		}
		else 
		{
			printf("no app.\r\n");
		}				
	}
}

void start_app(u8 type) /* 1: sram,  2: flash*/
{
	if(1 == type)
	{
		if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//判断是否为0X20XXXXXX.
		{
			printf(" startup app from sram.\r\n");
			iap_load_app(0X20001000);
		}
		else 
		{
			printf(" app in sram is error!\r\n");	   
		} 
	}
	else if(2 == type)
	{
		if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{
			printf(" startup app from flash.\r\n");
			iap_load_app(FLASH_APP1_ADDR);
		}
		else 
		{
			printf(" app in flash is error!\r\n");	   
		}									 	  
	}
	else
	{
		printf(" unknow app type!\r\n");
	}
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
	Shell_init();
	 
	while(1)
	{
		Shell_process();
		delay_ms(10);
	}  
}

