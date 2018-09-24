
#include "stm32f10x.h"
#include "systick_delay.h"
	 
static uint16_t u_facus = 0; 	/*ms延时倍乘数*/

/**
 * @brief  systick_init
 * @param  main_clk: defalut 72MHz
 * @retval none
 */
void systick_init(uint8_t main_clk)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  //选择时钟频率为HCLK/8=9M
	u_facus = main_clk>>3;		    
}

/**
 * @brief  ms延时函数
 * @param  nms:在系统时钟72M时，0<nms<=1864
 * @retval none
 * @note   SysTick->LOAD为24位寄存器,所以,最大延时为:
 *         nms<=0xffffff*8*1000/SYSCLK
 */								    
void systick_delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		
		
	if(nms == 0)
		return ;
	SysTick->LOAD = (uint32_t)nms * u_facus * 1000;	/*计数初值，倒计数*/
	SysTick->VAL  = 0x00;           				/*清空计数器*/
	SysTick->CTRL = 0x01 ;          	    		/*使能*/
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));		   
	SysTick->CTRL = 0x00;       		  			/*close*/
	SysTick->VAL  = 0X00;       		  	  	    
} 
  
/**
 * @brief  us延时函数
 * @param  nus:在系统时钟72M时，0<nus<=1864000
 * @retval none
 */									   
void systick_delay_us(uint32_t nus)
{		
	uint32_t temp;	   

	if(nus == 0)
		return;
	SysTick->LOAD = nus * u_facus; 			  		 
	SysTick->VAL  = 0x00;       				
	SysTick->CTRL = 0x01 ;      					 
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));	 
	SysTick->CTRL = 0x00;       				
	SysTick->VAL  = 0X00;       					 
}
