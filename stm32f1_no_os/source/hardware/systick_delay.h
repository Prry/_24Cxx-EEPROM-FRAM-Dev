#ifndef _SYSTICK_DELAY_H_
#define _SYSTICK_DELAY_H_
			   
#include <stdint.h>

extern void systick_init(uint8_t main_clk);	  
extern void systick_delay_ms(uint16_t nms);		 			 
extern void systick_delay_us(uint32_t nus);		 			 

#endif


