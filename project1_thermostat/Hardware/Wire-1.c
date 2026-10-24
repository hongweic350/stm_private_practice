#include "stm32f10x.h"               // Device header
#include "Delay.h"

void Wire_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void Wire_Out(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}
void Wire_Start(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   //每次都要切回推挽输出，
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;          //否则只有上电后第一次能拉低
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	Wire_Out();
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	Delay_ms(20);                          //起始低电平，DHT11 要求 >=18ms
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	Delay_us(30);                          //拉高 20~40us，然后释放总线
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}
uint8_t Wire_Check(void)//检测从机是否响应：等 80us 低 + 80us 高
{
	uint16_t a=0;

	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&&a<200)       //等 DHT11 把总线拉低
	{
		Delay_us(10);
		a+=10;
	}
	if(a>=200)
	return 0;

	a=0;
	while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&&a<200)      //等 80us 响应低电平结束
	{
		Delay_us(10);
		a+=10;
	}
	if(a>=200)
	return 0;

	a=0;
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&&a<200)       //等 80us 响应高电平结束
	{
		Delay_us(10);
		a+=10;
	}
	if(a>=200)
	return 0;

	return 1;
}
uint8_t Wire_IO(uint8_t data[5])
{
	uint8_t i;
	uint8_t Check;
	uint16_t a;

	for(i=0;i<5;i++)//进来先清零
	{
		data[i]=0;
	}

	Check = Wire_Check();
	if(Check==0)
	{
		return 0;//没应答直接打断函数
	}

	for(i=0;i<40;i++)
	{
		a=0;
		while(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&&a<200)  //等 50us 低电平结束（上升沿）
		{
			Delay_us(10);
			a+=10;
		}
		if(a>=200)
		return 0;

		Delay_us(40);                                          //40us 后仍为高就是 1，已变低就是 0

		data[i/8]=data[i/8]<<1;
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			data[i/8]|=1;

			a=0;
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&&a<200)//等 70us 高电平结束
			{
				Delay_us(10);
				a+=10;
			}
			if(a>=200)
			return 0;
		}
	}

	return 1;
}
