#include "main.h"
#include "stm32f10x_it.h"

#if	USE_CAN_EN
void USB_LP_CAN1_RX0_IRQHandler(void)
{// can中断接收函数
    CanRxMsg RxMessage;
		u8 i;
	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		if ((RxMessage.StdId & 0x100) != 0x100) 
				return; //该数据不是有效id段数据
		for(i = 0;i < CAN_MODULE_NUM;i++)
		{
				if(g_can1_object[i].module_id == (RxMessage.StdId & 0xFF) || RxMessage.StdId  == 0x100)
						put_cantx_to_fifobuff(&g_can1_object[i],(char *)RxMessage.Data,RxMessage.DLC);//将收到的数据放入CAN缓冲区
		}
		can_rx_led_state();
}
#endif

#if USE_USART1_EN 
void USART1_IRQHandler(void)
{// 串口1中断接收函数
    char Data; 
		if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET || (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET))   //判断中断标志
		{
				Data = USART_ReceiveData(USART1);
				put_usartrx_to_fifobuff(1,Data);
				USART_ClearITPendingBit(USART1, USART_IT_RXNE);  	// 清除中断标志位
		}
}
#endif

#if USE_USART2_EN 
void USART2_IRQHandler(void)
{// 串口2中断接收函数
    char Data; 
		if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET 
			|| (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET))   
		{
			Data = USART_ReceiveData(USART2);
			put_usartrx_to_fifobuff(2,Data);
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);  //清除中断标志位
		}
}
#endif

/*
#if USE_USART3_EN 
void USART3_IRQHandler(void)
{// 串口3中断接收函数
    char Data;        

		if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET || (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET))   
		{
				Data = USART_ReceiveData(USART3);
				put_usartrx_to_fifobuff(3,Data);
				USART_ClearITPendingBit(USART3, USART_IT_RXNE);  //清除中断标志位
		}
}
#endif
*/

void TIM2_IRQHandler(void)
{//10Ms  
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 		
	{		
		//led_work();
		 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);        
	}
}

void TIM3_IRQHandler(void)
{		
	
	if(TIM3->SR&0X0001)//if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)		
	{	
	
	}
}

void TIM6_IRQHandler(void)
{	
	
	if(TIM6->SR&0X0001)//if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)	
	{	
			
		TIM6->SR&=~(1<<0);	//TIM_ClearITPendingBit(TIM6, TIM_IT_Update);	
	}
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
