#include "main.h"

/***********************************************************
 * 函数名：Init_NVIC
 * 功能  ：外设中断设置    
 * 输入  : 中断向量偏移地址
 * 输出  ：无
 * 说明  : 
					NVIC_PreemptionPriority:抢占优先级
					NVIC_SubPriority       :响应优先级
					NVIC_Channel           :中断编号
					NVIC_Group             :中断分组 0~4
					注意优先级不能超过设定的组的范围!否则会有意想不到的错误
					组划分:
					组0:0位抢占优先级,4位响应优先级
					组1:1位抢占优先级,3位响应优先级
					组2:2位抢占优先级,2位响应优先级
					组3:3位抢占优先级,1位响应优先级
					组4:4位抢占优先级,0位响应优先级
					NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先
***********************************************************/
void Init_NVIC(FunctionalState state, u32 addr)
{    
	NVIC_InitTypeDef NVIC_InitStructure;	 

#ifdef  VECT_TAB_RAM  															//向量表基地址选择
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  		//将0x20000000地址作为向量表基地址(RAM)
#else  
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, addr); 	//将0x08000000地址作为向量表基地址(FLASH)  
#endif
				
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);	
 

#ifdef USE_USART1_EN 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_USART2_EN 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_USART3_EN 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

