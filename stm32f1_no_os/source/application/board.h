#ifndef _BOARD_H_
#define	_BOARD_H_
 
/*led port*/
#define 	LED_WORK_PORT   		GPIOC
#define 	LED_WORK_GPIO			GPIO_Pin_0
#define 	LED_WORK_RCC_PORT  		RCC_APB2Periph_GPIOC
#define		LED_WORK				PCout(0)

extern void wled_init(void);

#endif
