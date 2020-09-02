#ifndef __SIMULATE_UART_H
#define __SIMULATE_UART_H
#include "define_all.h"  


typedef enum
{
	s_Seven7Bit = 7,		//7位数据不支奇偶校验位
	s_Eight8Bit = 8,
	s_Nine9Bit = 9		//9位数据不支持奇偶校验位
	
}s_UART_DataBitTypeDef;

typedef enum
{
	s_NONE = 0,
	s_EVEN = 1,
	s_ODD = 2
	
}s_UART_ParityBitTypeDef;

typedef enum
{
	s_OneBit = 1 ,
	s_TwoBit = 2 
}s_UART_StopBitTypeDef;

typedef enum
{
	s_LSB = 0 ,
	s_MSB = 1 
}s_UART_SendFirstTypeDef;

typedef enum
{
	s_RIGHT = 0 ,
	s_WORNG = 1 
}s_UART_ReciveStateTypeDef;

typedef struct 
{
	GPIOx_Type*	Suart_TX_GPIO;
	uint32_t	Suart_TX_PIN;
	GPIOx_Type*	Suart_RX_GPIO;
	uint32_t	Suart_RX_PIN;
	uint32_t 	Suart_Baud_rate;							//波特率
	s_UART_DataBitTypeDef		Suart_DataBit;		//数据位数
	s_UART_ParityBitTypeDef	Suart_ParityBit; 		//校验位
	s_UART_StopBitTypeDef		Suart_StopBit;		//停止位
	s_UART_SendFirstTypeDef  Suart_SendFirst;
}Suart;


typedef struct 
{
	uint8_t start_bit;
	uint8_t recive_step;
	uint8_t recive_data;
	s_UART_ReciveStateTypeDef	Suart_ReciveState;
}Suart_Rx;

extern Suart suart;
extern Suart_Rx suart_rx;


#define	Suart_TX_INIT	OutputIO(suart.Suart_TX_GPIO, suart.Suart_TX_PIN , OUT_PUSHPULL);
#define	Suart_RX_INIT	InputtIO( suart.Suart_RX_GPIO, suart.Suart_RX_PIN,  IN_NORMAL );

#define 	Suart_TX_H	GPIO_SetBits(suart.Suart_TX_GPIO, suart.Suart_TX_PIN);
#define 	Suart_TX_L	GPIO_ResetBits(suart.Suart_TX_GPIO, suart.Suart_TX_PIN);

#define 	Suart_RX_H	GPIO_ReadInputDataBit( suart.Suart_RX_GPIO, suart.Suart_RX_PIN ) == Bit_SET 
#define 	Suart_RX_L	GPIO_ReadInputDataBit( suart.Suart_RX_GPIO, suart.Suart_RX_PIN ) == Bit_RESET 


#define	Suart_interval_time    1000000/suart.Suart_Baud_rate-6  //us




extern void Suart_SendByte(uint8_t val);
extern void Simulate_Uart_init(void);
extern void Suart_Sendstring(uint8_t* buf, uint32_t len);
extern void Suart_ReciveByte(void);

#endif
