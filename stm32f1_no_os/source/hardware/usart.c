#include "main.h"
#include "string.h"
#include "stdarg.h"

#define USE_UART_DEBUG 1
 /***********************************************************
 * 函数名：Init_USART
 * 功能  ：初始化串口配置     
 * 输入  : ComNum->USART0——3；Baudrate->波特率； 
 * 输出  ：无   
 ***********************************************************/
void usart_init(u8 UsartNum, u32 Baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	

	USART_InitStructure.USART_BaudRate            = Baudrate ;	  				//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 		//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 		//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式

	USART_ClockInitStructure.USART_Clock 					= USART_Clock_Disable;   					//USART时钟不使能
	USART_ClockInitStructure.USART_CPOL 					= USART_CPOL_Low;	      					//时钟极性
	USART_ClockInitStructure.USART_CPHA 					= USART_CPHA_2Edge;	      				//时钟相位
	USART_ClockInitStructure.USART_LastBit 				= USART_LastBit_Disable;
	switch(UsartNum)
	{
		case 1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 						//tx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	
			GPIO_Init(GPIOA, &GPIO_InitStructure);				   			
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 						//rx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
			GPIO_Init(GPIOA, &GPIO_InitStructure);				   				

			USART_ClockInit(USART1, &USART_ClockInitStructure);	 	
			USART_Init(USART1, &USART_InitStructure);							//根据参数初始化串口寄存器
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//使能串口1的发送和接收中断

			USART_Cmd(USART1, ENABLE);     											
		break;
		case 2:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 						//tx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  		//复用功能输出开漏
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
			GPIO_Init(GPIOA, &GPIO_InitStructure);				   				
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 						//rx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
			GPIO_Init(GPIOA, &GPIO_InitStructure);				   			

			USART_ClockInit(USART2, &USART_ClockInitStructure);	 	
			USART_Init(USART2, &USART_InitStructure);					  	
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//使能串口2的发送和接收中断
			USART_Cmd(USART2, ENABLE);   
		break;
		case 3:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
			GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);		//重映射usart3 到PC10 PC11
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 								//tx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   				//复用功能输出开漏
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   			
			GPIO_Init(GPIOC, &GPIO_InitStructure);				   					
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 								//tx
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//浮空输入(复位状态);	   				
			GPIO_Init(GPIOC, &GPIO_InitStructure);				   					

			USART_ClockInit(USART3, &USART_ClockInitStructure);	 		  
			USART_Init(USART3, &USART_InitStructure);					  			
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);						//使能串口3的发送和接收中断
			USART_Cmd(USART3, ENABLE);     								 						
		break;
		default:break;
	}
}

 /***********************************************************
 * 函数名：UsartSendByte
 * 功能  ：串口发送1字节   
 * 输入  : ComNum->USART0——3；Data->发送数据
 * 输出  ：无   
 ***********************************************************/
static void UsartSendByte(u8 UsartNum, char Data)
{
	switch(UsartNum)
	{
		case 1:
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 	//USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
			USART_SendData(USART1,Data);						  		 //发送一个字符
			USART_ClearFlag(USART1,USART_FLAG_RXNE);
		break;
		case 2:
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );
			USART_SendData(USART2, Data);
			USART_ClearFlag(USART2,USART_FLAG_RXNE);
		break;
		case 3:
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
			USART_SendData(USART3, Data);
			USART_ClearFlag(USART3,USART_FLAG_RXNE);
		break;
		default: break;
	}
}
/***********************************************************
 * 函数名：UsartSendByte
 * 功能  ：串口发送多字节   
 * 输入  : ComNum->USART0——3；Data->发送数据；Len->数据长度
 * 输出  ：无   
 ***********************************************************/
void UsartSendBytes(u8 UsartNum, char *Data, u16 Len)
{
	u16 i;

	for(i = 0; i < Len; i++)
	{
		UsartSendByte(UsartNum, Data[i]);
	}
}

#if USE_UART_DEBUG
/***********************************************************
 * 函数名：USART_send_char(u8 c)
 * 功能  ：串口发送一个字节数据 
 * 输入  : 1字节数据
 * 输出  ：无      
***********************************************************/
void USART_send_char(u8 c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );//等待发送完成
		USART_ClearFlag(USART3,USART_FLAG_RXNE);//清除发送标志
}																  

 /***********************************************************
 * 函数名：USART_send(const u8 *str,u8 length)
 * 功能  ：发送数据
 * 输入  : *str，发送数据； length，数据长度
 * 输出  ：无
 * 说明  ：发送给上位机时，数据格式是字符型       
 ***********************************************************/
void USART_send(u8 *str,u8 length)
{
    u8 temp;

	for (temp=0;temp<length;temp++)
	{ 
		USART_SendData(USART1,str[temp]);
		while (!(USART1->SR & USART_FLAG_TXE));					//等待发送完成
		USART_ClearITPendingBit(USART1,USART_FLAG_TC);			//清除发送标志
	}
}

 /***********************************************************
 * 函数名：USART_send(cosnt char *str)
 * 功能  ：发送一串字符
 * 输入  : *str，发送字符串
 * 输出  ：无
 * 说明  ：发送给上位机时，数据格式是字符型       
 ***********************************************************/
void USART_send_string(const u8 *str)
{
    u8 temp;

	for(temp=0;temp<strlen((char*)str);temp++)					//发送一串字符
	{ 
		USART_SendData(USART1,str[temp]);
		while (!(USART1->SR & USART_FLAG_TXE));					//等待发送完成
		USART_ClearITPendingBit(USART1,USART_FLAG_TC);			//清除发送标志
	}
}
//将整数转换为字符型,最大32位数据
char *IntToStr(signed int Value, char *String)				
{
    signed int  i;
		char	 d	  = 0;			//整数位
    char     High = 0;		   	//判断最高位是否为0
    char    *Ptr  = String;

    if (!Value)
    {
        *Ptr++ = 0x30;
        *Ptr = 0;
        return String;
    }
    if (Value < 0)
    {
        *Ptr++ = '-';
        Value *= -1;
    }

    for (i = 1000000000; i > 0; i /= 10)
    {
        d = Value / i;

        if (d || High)
        {
            *Ptr++ = (char)(d + 0x30);
            Value -= (d * i);
            High = 1;
        }
    }

    *Ptr = 0;
    return String;
} 
/***********************************************************
 * 函数名：*itoa(int value, char *string, int radix)
 * 功能  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10 
 * 输出  ：无    
 ***********************************************************/
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    if (value < 0)
    {
        *ptr++ = '-';
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    *ptr = 0;
    return string;
} 

/***********************************************************
 * 函数名：USART_printf(USART_TypeDef* USARTx, u8 *Data,...)
 * 功能  ：串口打印
 * 输入  : 串口号、数据
 * 输出  ：无
 * 调用  ：*itoa(int value, char *string, int radix)
 * 被调用：
 * 说明  ：典型应用  UART_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 UART_printf( USART1, "\r\n %d \r\n", i );
 *            		 UART_printf( USART1, "\r\n %s \r\n", j );       
 ***********************************************************/
void USART_printf(USART_TypeDef* USARTx, u8 *Data,...)
{
	const char *s;
    int d;   
    char buf[16];

    va_list ap;
    va_start(ap, Data);

	while ( *Data != 0)       // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':    //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':   //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  
			switch ( *++Data )
			{				
				case 's':	//字符串
        					s = va_arg(ap, const char *);
                            for ( ; *s; s++) 
                            {
                                USART_SendData(USARTx,*s);
                                while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                            }
                            Data++;
                            break;

                case 'd':  //十进制
                            d = va_arg(ap, int);
                            itoa(d, buf, 10);  //字符转换为数字
                            for (s = buf; *s; s++) 
                            {
                                USART_SendData(USARTx,*s);
                                while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
                            }
                            Data++;
                            break;
                default:
						Data++;
                        break;
			}		 
		} 
		else 
				USART_SendData(USARTx, *Data ++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
#endif
