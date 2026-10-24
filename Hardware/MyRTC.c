#include "stm32f10x.h"                  // Device header
#include "Delay.h"                      // 新增：LSE 超时计数要用 Delay_ms
#include <time.h>

struct tm* Time;
void MyRTC_Init(void)
{
	uint32_t i = 0;                     // 新增：等 LSE 起振用了多久（毫秒）
	uint32_t Prescaler = 32768 - 1;     // 新增：LSE 32768Hz 就减 1，退到 LSI 时再改

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5)//可以是任何数，防止再次初始化，顶掉原来的计数
	{
		RCC_LSEConfig(RCC_LSE_ON);

		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET && i<3000)//等 LSE 真起振，最多 3 秒
		{
			Delay_ms(1);
			i++;
		}

		if(i>=3000)//3 秒还没好：板上多半没焊 32.768k 晶振，退用内部 LSI，别在这儿吊死
		{
			RCC_LSEConfig(RCC_LSE_OFF);
			RCC_LSICmd(ENABLE);
			while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)!=SET);
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
			Prescaler = 40000 - 1;
		}
		else
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		}

		RCC_RTCCLKCmd(ENABLE);
	
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(Prescaler);
		RTC_WaitForLastTask();
		
		RTC_SetCounter(1792822954);//2026-10-23 22:51:00，换掉 time()：它是半主机调用，没调试器就 HardFault
		RTC_WaitForLastTask();
		
		BKP_WriteBackupRegister(BKP_DR1,0xA5);
	}
	else
	{
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	}
}
void MyRTC_GetTime(void)
{
	time_t tm;
	tm =RTC_GetCounter();
	Time=localtime(&tm);

}
