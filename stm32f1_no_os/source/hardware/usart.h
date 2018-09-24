#ifndef			__USART_H__
#define			__USART_H__

extern void usart_init(u8 UsartNum, u32 Baudrate);
extern void UsartSendBytes(u8 UsartNum, char *data, u16 len);
extern void USART_send_char(u8 c);				 				//发送一个字节数据
extern void USART_send(u8 *str,u8 length);       	//发送一定长度的数据
extern void USART_send_string(const u8 *str);	 		//发送一串字符串
extern void USART_printf(USART_TypeDef* USARTx, u8 *Data,...);

#endif

