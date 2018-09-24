
/*
 * File      : time.c
 * 
 * This file is part of timer for stm32f1.
 * COPYRIGHT (C) 2014
 *
 * Change Logs:
 * Date           Author       Version			Notes
 * 2014-09-04     Acuity      	1.00		first version.
 *
 * Depend on:
 * st lib
 *
 * Note:
 * 
 */
 
#include "main.h"

/***********************************************************
 * 函数名：Init_Timer
 * 功能  ：初始化定时器计数      
 * 输入  : 定时器n,定时时间,时间单位， IsEnable 开启或关闭定时器
 * 输出  ：无
 * 说明  : >= Nus >=1  t = (TIM_Period + 1)*(TIM_Prescaler + 1)/T  T=72MHz
 *       ：输入时间单位是n_us时,范围是1-910us，n_ms时，范围是1-6553ms
***********************************************************/
void Init_Timer(TIM_TypeDef *TIMx,u32 times,u8 unit,FunctionalState IsEnable)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;		
	if (TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   	
	}
	else if (TIMx == TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   	
	}
	else if (TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   	
	}	
	else if (TIMx == TIM4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
	}
	else if (TIMx == TIM5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);   		
	}
	else if (TIMx == TIM6)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);   	
	}
	TIM_DeInit( TIMx);                             //定时器初始化位复位值
	TIM_InternalClockConfig(TIMx); 															//配置IMx内部时钟
	switch(unit)
	{
		case n_motor:
		{
			TIM_BaseInitStructure.TIM_Period        = times; 	//设置自动重载寄存器值为最大值	0~65535之间  10ms——>20	2——>2											
			TIM_BaseInitStructure.TIM_Prescaler     = 10;  		 		
		}	
		break;
		case n_us:
		{	   
			TIM_BaseInitStructure.TIM_Period        = times * 72 -1; //设置自动重载寄存器值为最大值	0~65535之间  10ms——>20	2——>2											
			TIM_BaseInitStructure.TIM_Prescaler     = 0;  		     //不分频										
		}
		break;
		case n_ms:
		{
			TIM_BaseInitStructure.TIM_Period        = times * 10 -1; //设置自动重载寄存器值为最大值	0~65535之间  10ms——>20	2——>2											
			TIM_BaseInitStructure.TIM_Prescaler     = 7200-1;		 		//分频，10khz
		}	
		break;
		default:break;
	}
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	 //时钟分割为0
	TIM_BaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up; //TIM向上计数模式 从0开始向上计数，计数到1000后产生更新事件
	if ((TIMx == TIM1 || TIMx == TIM8))	
	{
		TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		  //此参数只适合于TIM1和TIM8
	}
	TIM_TimeBaseInit(TIMx, &TIM_BaseInitStructure); 			  //根据指定参数初始化TIM时间基数寄存器
	TIM_ClearFlag(TIMx, TIM_FLAG_Update); 					      //清中断，以免一启用中断后立即产生中断  
	TIM_ARRPreloadConfig(TIMx, IsEnable);					      //禁止ARR 预装载缓冲器						 
	TIM_IT_EN(TIMx,IsEnable);								      						//使能定时
}

/***********************************************************
 * 函数名：TIM_IT_EN(TIM_TypeDef* TIMx,FunctionalState state)
 * 功能  ：打开或关闭定时器 计数中断  
 * 输入  : TIMx,定时器x  state,开启或关闭状态(ENABLE or DISABLE)
 * 输出  ：无      
 ***********************************************************/
void TIM_IT_EN(TIM_TypeDef* TIMx,FunctionalState state)
{
	TIM_ITConfig(TIMx,TIM_IT_Update,state);				  //使能或失能中断
	TIM_Cmd(TIMx, state);  								   	   					//使能TIMx外设 
}

#define USE_PWM_OUT 1
#if USE_PWM_OUT
/***********************************************************
 * 函数名：Init_PWM
 * 功能  ：定时器产生PWM        
 * 输入  : TIMx->定时器  channel->PWM通道  rate->频率  cycle->占空比  EN_state->是否使能该通道输出 
 * 输出  ：无
 * 说明  ：void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2)
***********************************************************/

void Init_PWM_Out(TIM_TypeDef *TIMx,u8 channel,u32 rate,u16 cycle,FunctionalState IsEnable)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	if (TIMx == TIM1)
	{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);		//定时器1时钟使能
	}
	else if (TIMx == TIM2)
	{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//定时器2时钟使能
	}
	else if (TIMx == TIM3)
	{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//定时器3时钟使能
	}
	else if (TIMx == TIM4)
	{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		//定时器4时钟使能
	}
	else if (TIMx == TIM5)
	{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);		//定时器5时钟使能
	}
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 	//增计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		    //不分频

	//根据频率大小设置时钟分频数
	if (rate > 1000)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 0;											//预分频系数为0
		TIM_TimeBaseStructure.TIM_Period = 72000000 / rate - 1;				//hz	
	}
	else if ((rate > 10) && (rate <= 1000))
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 99;											//预分频系数为
		TIM_TimeBaseStructure.TIM_Period = 72000000 / (rate * 100) - 1;	
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 999;										//预分频系数为
		TIM_TimeBaseStructure.TIM_Period = 72000000 / (rate * 1000) - 1;	
	}
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			 	
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;										//PWM模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	if (rate > 1000)
	{
		TIM_OCInitStructure.TIM_Pulse = 720000 * cycle / rate ;					//占空比
	}
	else if ((rate > 10) && (rate <= 1000))	   
	{
		TIM_OCInitStructure.TIM_Pulse = 720000 * cycle / (rate *100) ;	//占空比
	}
	else
	{
		TIM_OCInitStructure.TIM_Pulse = 720000 * cycle / (rate *1000) ;	//占空比	
	}
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	      		//输出极性高
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{	//高级定时器特殊设置
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;	//CHN禁止输出
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	}	   
	switch (channel)
	{
		case 1:
		{
				TIM_OC1Init(TIMx, &TIM_OCInitStructure);							//PA.8通道1PWM
				TIM_OC1PreloadConfig(TIMx,TIM_OCPreload_Enable);  		//通道1使能
		} 
		break;
		case 2:
		{
				TIM_OC2Init(TIMx, &TIM_OCInitStructure);
				TIM_OC2PreloadConfig(TIMx,TIM_OCPreload_Enable);  		//通道2使能	
		}   
		break;
		case 3:
		{
				TIM_OC3Init(TIMx, &TIM_OCInitStructure);
				TIM_OC3PreloadConfig(TIMx,TIM_OCPreload_Enable);  		//通道3使能	
		}   
		break;
		case 4:
		{
				TIM_OC4Init(TIMx, &TIM_OCInitStructure);							//PA.11通道4pwm
				TIM_OC4PreloadConfig(TIMx,TIM_OCPreload_Enable);  		//通道4使能	
		}   
		break;
		default:
		break;
	}																    
	TIM_PWM_EN(TIMx,IsEnable);						    										//允许重装载
	if ((TIMx == TIM1) || TIMx == TIM8)
	{//高级定时器特殊设置
			TIM_CtrlPWMOutputs(TIMx, IsEnable);												//高级定时器1和定时器8特用
	}
}

/***********************************************************
 * 函数名：TIM_PWM_EN(TIM_TypeDef* TIMx,FunctionalState state)
 * 功能  ：打开或关闭定时器输出PWM   
 * 输入  : TIMx,定时器x  state,开启或关闭状态(ENABLE or DISABLE)
 * 输出  ：无      
 ***********************************************************/
void TIM_PWM_EN(TIM_TypeDef* TIMx,FunctionalState state)
{
	TIM_Cmd(TIMx, state);						//使能TIMx外设    
	TIM_ARRPreloadConfig(TIMx,state); 			//重装初值 
}

/***********************************************************
 * 函数名：PWM_Set_Rate(TIM_TypeDef* TIMx,u16 rate)
 * 功能  ：改变PWM输出频率 
 * 输入  : TIMx定时器x，rate频率（Hz）
 * 输出  ：无  
 * 说明  ：暂未能实现！！！！！！！，不能用    
 ***********************************************************/
void PWM_Set_Rate(TIM_TypeDef* TIMx,u16 rate)
{	
	u16 peroid = 0;
	
	TIM_PWM_EN(TIMx,DISABLE);
	if (rate >1000)
	{
			peroid = 72000000 / rate - 1;			  //频率不分频
	}
	else if ((rate > 10) && (rate <= 1000))
	{
			peroid = 72000000 / (rate * 100) - 1;//720000 / rate - 1;//频率99分频，72000000 / (rate * 100) - 1;
	}
	else
	{
		peroid = 72000000 / (rate * 1000) - 1;//72000 / rate -1;//频率999分频，72000000 / (rate * 100) - 1;
	}
	TIM_SetAutoreload(TIMx,peroid);
	TIM_PWM_EN(TIMx,ENABLE);
	TIM_CtrlPWMOutputs(TIMx,ENABLE);							   //高级定时器1和定时器8特用
}

/***********************************************************
 * 函数名：PWM_Set_Cycl(TIM_TypeDef* TIMx,u16 cycle)
 * 功能  ：改变PWM输出占空比 
 * 输入  : TIMx定时器x，rate当前PWM频率，channel输出通道，cycl占空比0-100
 * 输出  ：无      
 ***********************************************************/

void PWM_Set_Cycle(TIM_TypeDef* TIMx,u8 channel,u32 rate,u16 cycle)
{
	u16 pulse = 0;
	if (rate > 1000)
	{
		pulse = 720000 * cycle / rate ;			//占空比
	}
	else if ((rate > 10) && (rate <= 1000))	   
	{
		pulse = 720000 * cycle / (rate *100) ;	//占空比
	}
	else
	{
		pulse = 720000 * cycle / (rate *1000) ;	//占空比	
	}
	switch(channel)
	{
		case 1:
			TIM_SetCompare1(TIMx,pulse);
		break;
		case 2:
			TIM_SetCompare2(TIMx,pulse);
		break;
		case 3:
			TIM_SetCompare3(TIMx,pulse);
		break;
		case 4:
			TIM_SetCompare4(TIMx,pulse);
		break;
		default:

		break;		 	
	}	
}
#endif

#if USE_PWM_CAPTURE
/***********************************************************
 * 函数名：Init_PWM_Capture
 * 功能  ：定时PWM输入，常用于电机测速       
 * 输入  : channel->  TIM_Channel_x(x->1-4)
 * 输出  ：无
 * 说明	 ：使用时必须在nvic.c源文件设置中断优先级
***********************************************************/
void Init_PWM_Capture(TIM_TypeDef *TIMx,u8 channel,u16 rate,FunctionalState IsEnable)
{
	TIM_ICInitTypeDef   		TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	if (TIMx == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if (TIMx == TIM3)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_DeInit(TIMx);
  	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;     					//周期
  	TIM_TimeBaseStructure.TIM_Prescaler = 999;      				//时钟分频->n+1
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;   					//时钟分割
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计算模式
  	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);					//初始化
	
		TIM_ICInitStructure.TIM_Channel = channel;   						//通道选择                
  	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     //上升沿捕获
  	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //管脚与寄存器关系
  	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV4;           //四分频
  	TIM_ICInitStructure.TIM_ICFilter = 0x0;      					//滤波设置，经历几个周期跳变认定波形稳定0x0～0xF

  	TIM_PWMIConfig(TIMx, &TIM_ICInitStructure);                 	
  	TIM_SelectInputTrigger(TIMx, TIM_TS_TI2FP2);                	//TIM经滤波定时器输入2
  	TIM_SelectSlaveMode(TIMx, TIM_SlaveMode_Reset);								//选中触发信号的上升沿重初始化计数器并触发寄存器更新
  	TIM_SelectMasterSlaveMode(TIMx, TIM_MasterSlaveMode_Enable); 	//主从模式使能，定时器被动触发
  	TIM_Cmd(TIMx, ENABLE);                                 			//使能定时器
	switch(channel) 
	{       
		case 1:
			TIM_ITConfig(TIMx, TIM_IT_CC1, ENABLE);     								//通道使能
		break;
		case 2:
			TIM_ITConfig(TIMx, TIM_IT_CC2, ENABLE);  
		break;
		case 3:
			TIM_ITConfig(TIMx, TIM_IT_CC3, ENABLE); 
		break;
		case 4:
			TIM_ITConfig(TIMx, TIM_IT_CC4, ENABLE);  
		break; 
	}
}
#endif
