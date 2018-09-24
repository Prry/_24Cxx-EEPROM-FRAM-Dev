#ifndef _TIME_H_
#define	_TIME_H_

enum
{
	n_motor = 0,
	n_us,
	n_ms,
	n_s,
};

extern void Init_Timer(TIM_TypeDef *TIMx,u32 times,u8 unit,FunctionalState IsEnable);
extern void TIM_IT_EN(TIM_TypeDef* TIMx,FunctionalState state); 			//开启或关闭定时器计数中断
extern void Init_PWM_Out(TIM_TypeDef *TIMx,u8 channel,u32 rate,u16 cycle,FunctionalState IsEnable);
extern void TIM_PWM_EN(TIM_TypeDef* TIMx,FunctionalState state);			//开启或关闭定时PWM输出
extern void PWM_Set_Rate(TIM_TypeDef* TIMx,u16 rate);						//设定PWM频率
extern void PWM_Set_Cycle(TIM_TypeDef* TIMx,u8 channel,u32 rate,u16 cycle);	//设定PWM占空比
extern void Init_PWM_Capture(TIM_TypeDef *TIMx,u8 channel,u16 rate,FunctionalState IsEnable);

#endif
