/*
 * File      : main.c
 * 
 *  
 * COPYRIGHT (C) 2018
 *
 * Change Logs:
 * Date           Author       Version			Notes
 * 2018-09-01     Acuity      	1.00		first version.
 *
 * 
 * Note:
 * 
 */

#define 	APP_SHIFT_ADDR		0x0000	/*中断向量表偏移地址*/
#include "main.h"

int main(void)
{
	__enable_irq();	
	SystemInit();
	systick_init(72);	
	Init_NVIC(ENABLE, APP_SHIFT_ADDR);
	
	stm32f1xx_i2c_init();	/*i2c init*/

	{/*eeprom test*/
		extern int16_t _24cxx_test(void);
		_24cxx_test();
	}
	for(;;)
	{

	}													
}

