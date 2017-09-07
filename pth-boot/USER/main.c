#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "sram.h"
#include "iap.h"
#include "shell.h"

/*app store in 128k inner sram, 
*reserve start 4k for pth-boot, 
*reserve end 24k for user memory,
*hept, 2017.9.7 */
#define SRAM_APP_MAX_LEN  			100*1024 

void load_app(u8 type)  /* 1: sram,  2: flash*/
{
    u32 oldcount=0;
    u32 applenth=0;
	u8 * p = NULL;
	u8 i = 0;

    printf(" start receive app, please don't typewrite!\r\n");
    uart_interrupt_init(1);

	printf(" received 0x%08X Byte", oldcount);
    while(1)
    {
        if(USART_RX_CNT)
        {
            if(oldcount == USART_RX_CNT)
            {
                applenth = USART_RX_CNT;
                oldcount = 0;
                USART_RX_CNT = 0;
                break;
            }
            else
            {
                oldcount=USART_RX_CNT;
            }
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
			printf("%08X Byte", oldcount);
        }

        delay_ms(10);
    }

	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
	printf("%08X Byte\r\n", applenth);

    uart_interrupt_init(0);

    if(applenth)
    {
		if(1 == type)
		{
			if(applenth > SRAM_APP_MAX_LEN)
			{
				printf(" received file larger than sram size %dKB.\r\n", SRAM_APP_MAX_LEN/1024);
			}
            else if(((*(vu32*)(USART_RX_BUF+4))&0xFF000000) == 0x20000000)//判断是否为0X20XXXXXX.
            {
                printf(" write app to sram...");
				p = (u8*)0X20001000;
				for(i = 0; i < applenth; p ++, i ++)
				{
					*p = USART_RX_BUF[i];
				}
                printf("\b\b\b ok!\r\n");
            }
            else
            {
                printf(" received file is not sram app.\r\n");
            }
        }
        else if(2 == type)
        {
            if(((*(vu32*)(USART_RX_BUF+4))&0xFF000000) == 0x08000000)//判断是否为0X08XXXXXX.
            {
                printf(" write app to flash...");
                iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);
                printf("\b\b\b ok!\r\n");
            }
            else
            {
                printf(" received file is not flash app.\r\n");
            }
        }
    }
    else
    {
        printf(" no app.\r\n");
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
    FSMC_SRAM_Init();
    Shell_init();

    while(1)
    {
        Shell_process();
        delay_ms(10);
    }
}

