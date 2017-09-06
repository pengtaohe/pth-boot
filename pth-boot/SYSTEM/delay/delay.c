#include "delay.h"
#include "sys.h"

static u8  fac_us=0;							//usÑÓÊ±±¶³ËÊı
static u16 fac_ms=0;
			   
//³õÊ¼»¯ÑÓ³Ùº¯Êı
//µ±Ê¹ÓÃOSµÄÊ±ºò,´Ëº¯Êı»á³õÊ¼»¯OSµÄÊ±ÖÓ½ÚÅÄ
//SYSTICKµÄÊ±ÖÓ¹Ì¶¨ÎªAHBÊ±ÖÓµÄ1/8
//SYSCLK:ÏµÍ³Ê±ÖÓÆµÂÊ
void delay_init(u8 SYSCLK)
{
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;						//²»ÂÛÊÇ·ñÊ¹ÓÃOS,fac_us¶¼ĞèÒªÊ¹Ó
	fac_ms=(u16)fac_us*1000;
}								    

//ÑÓÊ±nus
//nusÎªÒªÑÓÊ±µÄusÊı.	
//×¢Òâ:nusµÄÖµ,²»Òª´óÓÚ798915us(×î´óÖµ¼´2^24/fac_us@fac_us=21)
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//Ê±¼ä¼ÓÔØ	  		 
	SysTick->VAL=0x00;        				//Çå¿Õ¼ÆÊıÆ÷
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //¿ªÊ¼µ¹Êı 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´ıÊ±¼äµ½´ï   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //¹Ø±Õ¼ÆÊıÆ÷
	SysTick->VAL =0X00;       				//Çå¿Õ¼ÆÊıÆ÷ 
}
//ÑÓÊ±nms
//×¢ÒânmsµÄ·¶Î§
//SysTick->LOADÎª24Î»¼Ä´æÆ÷,ËùÒÔ,×î´óÑÓÊ±Îª:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLKµ¥Î»ÎªHz,nmsµ¥Î»Îªms
//¶Ô168MÌõ¼şÏÂ,nms<=798ms 
void delay_xms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;			//Ê±¼ä¼ÓÔØ(SysTick->LOADÎª24bit)
	SysTick->VAL =0x00;           			//Çå¿Õ¼ÆÊıÆ÷
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //¿ªÊ¼µ¹Êı 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//µÈ´ıÊ±¼äµ½´ï   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //¹Ø±Õ¼ÆÊıÆ÷
	SysTick->VAL =0X00;     		  		//Çå¿Õ¼ÆÊıÆ÷	  	    
} 
//ÑÓÊ±nms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	u8 repeat=nms/540;						//ÕâÀïÓÃ540,ÊÇ¿¼ÂÇµ½Ä³Ğ©¿Í»§¿ÉÄÜ³¬ÆµÊ¹ÓÃ,
											//±ÈÈç³¬Æµµ½248MµÄÊ±ºò,delay_xms×î´óÖ»ÄÜÑÓÊ±541ms×óÓÒÁË
	u16 remain=nms%540;
	while(repeat)
	{
		delay_xms(540);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 

