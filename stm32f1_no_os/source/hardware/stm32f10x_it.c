#include "main.h"
#include "stm32f10x_it.h"

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	
}

void USART1_IRQHandler(void)
{
	
}

void USART2_IRQHandler(void)
{
	
}

void TIM2_IRQHandler(void)
{ 
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 		
	{		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);        
	}
}

void TIM3_IRQHandler(void)
{		

}

void TIM6_IRQHandler(void)
{	
	
}
void NMI_Handler(void)
{
	
}

void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{

	}
}

void MemManage_Handler(void)
{
		/* Go to infinite loop when Memory Manage exception occurs */
		while (1)
		{

		}
}

void BusFault_Handler(void)
{
		/* Go to infinite loop when Bus Fault exception occurs */
		while (1)
		{
			
		}
}

void UsageFault_Handler(void)
{
		/* Go to infinite loop when Usage Fault exception occurs */
		while (1)
		{

		}
}

void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}

void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{

}
