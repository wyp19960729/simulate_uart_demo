#include "define_all.h" 
#define   max_recive_len    64
unsigned char test_buf[max_recive_len];
static unsigned int bufpoint = 0;
Suart_Rx suart_rx;
Suart suart=
{
	GPIOA,
	GPIO_Pin_12,
	GPIOA,
	GPIO_Pin_14,
	9600,
	s_Eight8Bit,
	s_EVEN,
	s_OneBit,
	s_MSB,
};

void GPIO_IRQHandler(void)
{
	if(SET == GPIO_EXTI_EXTIxIF_ChkEx(GPIOA, GPIO_Pin_14))
	{
		IWDT_Clr();             
		Suart_ReciveByte();
//		if( suart_rx.Suart_ReciveState != s_WORNG)Suart_SendByte(suart_rx.recive_data);
		GPIO_EXTI_EXTIxIF_ClrEx(GPIOA, GPIO_Pin_14);
	}
}
	
void Suart_GPIO_INI(void)
{
	RCC_PERCLK_SetableEx(EXTI2CLK, ENABLE);		//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(EXTI1CLK, ENABLE);		//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(EXTI0CLK, ENABLE);		//EXTI外部引脚中断采样时钟，IO数字滤波时钟使能
	RCC_PERCLK_SetableEx(PDCCLK, ENABLE);		//IO控制时钟寄存器使能
	
	Suart_RX_INIT;	
	
	//FM33A0XX的7组GPIO（A~G）最多可以产生24个外部引脚中断，部分IO不能同时开启中断功能
	GPIO_EXTI_Init( GPIOA, GPIO_Pin_14, EXTI_FALLING);	//PF5输入中断打开
	
    /*NVIC中断配置*/
	NVIC_DisableIRQ(GPIO_IRQn);
	NVIC_SetPriority(GPIO_IRQn,2);//中断优先级配置
	NVIC_EnableIRQ(GPIO_IRQn);
}

void Simulate_Uart_init()
{
	Suart_TX_INIT;
	Suart_TX_H;
	Suart_GPIO_INI();
}


void Suart_SendByte(uint8_t val)
{
    unsigned int  i,odd_even_bit = 0;

    Suart_TX_L;
    TicksDelayUs(Suart_interval_time);

    for(i=0;i<suart.Suart_DataBit;i++)
    {
		if( suart.Suart_SendFirst == s_LSB)
		{
			 if(val&0x1)
			{
			   Suart_TX_H;
				odd_even_bit++;
			}
			else
			{
				Suart_TX_L;
			}
			val>>=1;
		}
		else
		{
			if(val&0x80)
			{
			   Suart_TX_H;
				odd_even_bit++;
			}
			else
			{
				Suart_TX_L;
			}
			val<<=1;
		}
        TicksDelayUs(Suart_interval_time);
    }
	switch( suart.Suart_ParityBit)
	{
		case s_NONE:
			break;
		case s_EVEN:
			if(odd_even_bit%2 == 0) {Suart_TX_L;}
			else Suart_TX_H;
			break;
		case s_ODD:
			if(odd_even_bit%2 == 0)  {Suart_TX_H;}
			else Suart_TX_L;
			break;
		default:
			break;
			
	}
	TicksDelayUs(Suart_interval_time);
    for(i=0;i<suart.Suart_StopBit;i++)
    {
		Suart_TX_H;
		TicksDelayUs(Suart_interval_time);
	}
}

void Suart_Sendstring(uint8_t* buf, uint32_t len)
{
	int i;
	for(  i = 0;i < len ;i++)
	{
		IWDT_Clr(); 
		Suart_SendByte(buf[i]);
	}
}


void Suart_ReciveByte(void)
{
	uint8_t i,odd_even_bit = 0;
	suart_rx.recive_data = 0;
	suart_rx.Suart_ReciveState = s_RIGHT;
	if(!suart_rx.start_bit)
	{
		if( Suart_RX_L) 
		{
			suart_rx.start_bit = 1;
			if( suart.Suart_SendFirst == s_LSB)
			{
				suart_rx.recive_step = 0;
			}
			else suart_rx.recive_step = suart.Suart_DataBit-1;
		}
		else suart_rx.start_bit = 0;
		if(suart_rx.start_bit )
		{
			TicksDelayUs(Suart_interval_time);
		   for(i=0;i<suart.Suart_DataBit;i++)
			{
				if(Suart_RX_H)
				{
					suart_rx.recive_data |= 1 << suart_rx.recive_step;
					odd_even_bit++;
				}
				if( suart.Suart_SendFirst == s_LSB)
				{
					suart_rx.recive_step++;
				}
				else suart_rx.recive_step--;
				
				TicksDelayUs(Suart_interval_time);
			}
			switch( suart.Suart_ParityBit)
			{
				case s_NONE:
					break;
				case s_EVEN:
					if( (odd_even_bit%2  == 0) && ( Suart_RX_L )  );
					else if( !(odd_even_bit%2 == 0 ) && ( Suart_RX_H )  );
					else suart_rx.Suart_ReciveState = s_WORNG;
					break;
				case s_ODD:
					if( (odd_even_bit%2 == 0) && ( Suart_RX_H )  );
					else if( !(odd_even_bit%2 == 0 ) && ( Suart_RX_L )  );
					else suart_rx.Suart_ReciveState = s_WORNG;
					break;
				default:
					break;
			}
			TicksDelayUs(Suart_interval_time);
			for(i=0;i<suart.Suart_StopBit-1;i++)
			{
				TicksDelayUs(Suart_interval_time);
			}
			if( suart_rx.Suart_ReciveState != s_WORNG) 
			{
				test_buf[ bufpoint ] = suart_rx.recive_data;
				bufpoint++;
				if( bufpoint >= max_recive_len) bufpoint = 0;
			}
			suart_rx.start_bit = 0;
			suart_rx.recive_step = 0;
		}
	}

}
















