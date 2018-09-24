#include "main.h"

void wled_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_WORK_RCC_PORT ,ENABLE);
	GPIO_InitStructure.GPIO_Pin  	= LED_WORK_GPIO;  			
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;	        
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(LED_WORK_PORT, &GPIO_InitStructure);

	LED_WORK = 0;		
}
