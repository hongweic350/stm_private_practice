#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "DHT11.h"
#include "Key.h"
#include "W25Q64.h"
#include "MyRTC.h"
/**
  * 实战项目1 · 桌面温控时钟
  *
  * 页面用 Mode 变量记住，不再靠"按键此刻是否按下"：
  *   Mode 0 -> 温湿度
  *   Mode 1 -> 时钟
  *   Mode 2 -> 最值
  *
  * K1 = 下一页，K2 = 上一页（循环切换）
  */
uint8_t Ack;
uint8_t Data[5];
uint32_t AddressPlus=0;

#define MODE_NUM	3		//一共几个页面

/* 把 Data[] 里的温湿度画出来（不读传感器，只负责显示） */
void ShowData(void)
{
	OLED_ShowNum(1,10,Data[0],2);
	OLED_ShowNum(1,13,Data[1],1);
	OLED_ShowString(1,12,".");
	OLED_ShowNum(2,6,Data[2],2);
	OLED_ShowNum(2,9,Data[3],1);
	OLED_ShowString(2,8,".");
}
float math_Max(float num1,float num2) //比大小都封装
{
	if(num1>num2)
	{
		return num1;
	}
	else return num2;
}
float math_Min(float num1,float num2) //比大小都封装
{
	if(num1>num2)
	{
		return num2;
	}
	else return num1;

}
int main(void)
{
	uint8_t Mode=0;			//当前页面
	uint8_t DrawFlag=1;		//1 = 需要重画这一页
	uint32_t Tick=0;		//50ms 节拍计数，30 次 = 1.5 秒采一次 DHT11

	
	OLED_Init();
	PWM_Init();
	DHT11_Init();
	Key_Init();
	W25Q64_Init();
	MyRTC_Init();
	Delay_ms(1000);//开始延迟1秒防止DHT11检测的不准值被投影
	
	W25Q64_SectorErase(0X000000);
	while (1)
	{	
		
		/* ---------- 按键：只在这里改 Mode ---------- */
		uint8_t Key=Key_GetNum();
		if(Key==1)
		{
			Mode=(Mode+1)%MODE_NUM;		//K1 下一页
			DrawFlag=1;
		}
		if(Key==2)
		{
			Mode=(Mode+MODE_NUM-1)%MODE_NUM;	//K2 上一页
			DrawFlag=1;
		}

		/* ---------- 换页时清屏 + 画标题，只画一次 ---------- */
		if(DrawFlag)
		{
			OLED_Clear();
			if(Mode==0)
			{
				OLED_ShowString(1,1,"humidity:");
				OLED_ShowString(2,1,"heat:");
				if(Ack==1)				//直接用上次读到的数，不重新采样
				{
					ShowData();
				}
			}
			else if(Mode==1)
			{
				OLED_ShowString(1,1,"TimeMode:");
			}
			else
			{
			
				OLED_ShowString(2,1,"humid");
				OLED_ShowString(3,1,"heat");
				OLED_ShowString(1,5,"max");
				OLED_ShowString(1,10,"min");
			}
			DrawFlag=0;
		}

		/*各个模式的执行层*/
		if(Mode==0)//模式检测
		{
			if(Tick==0)						//每 1.5 秒采一次，节拍，延迟1s
			{
				Ack= DHT11_ReadData(Data);
				if(Ack==1)//是否有应答
				{	
					if(Data[4]==(uint8_t)(Data[0]+Data[1]+Data[2]+Data[3]))//校验和一致才算这一帧有效（原来是反的）
					{
						
						ShowData();
						OLED_ShowString(4,1,"          ");//"Data wrong"是10个字符，只擦5个会留"wrong"

						/* 用这一帧的当前温度直接控制，不再翻 Flash 里的历史记录 */
						if(Data[2]+0.1*Data[3]>=27.0)
						{
							PWM_SetCompare(50);
						}
						else
						{
							PWM_SetCompare(0);
						}

						if(AddressPlus+5<=4096)			//只用一个 4KB 扇区：写满就停，别写进没擦过的区域
						{
							W25Q64_Programming(AddressPlus,Data,5);
							AddressPlus+=5;
						}
					}
					else
					{
						OLED_ShowString(4,1,"Data wrong");
					}
				}
				else
				{
					OLED_ShowString(4,1,"No Ack");
				}
			}
		}
		else if(Mode==1)/*显示 RTC 时间 */
		{
			OLED_ShowString(1,1,"year");
			OLED_ShowString(1,9,"month:");
			OLED_ShowString(2,1,"day:");
			OLED_ShowString(2,7,"hour:");
			OLED_ShowString(3,1,"min:");
			OLED_ShowString(3,7,"sec:");
			
			MyRTC_GetTime();	
		
			OLED_ShowNum(1,5,Time->tm_year+1900,4);
			OLED_ShowNum(1,15,Time->tm_mon+1,2);
			OLED_ShowNum(2,5,Time->tm_mday,2);
			OLED_ShowNum(2,13,Time->tm_hour,2);
			OLED_ShowNum(3,5,Time->tm_min,2);
			OLED_ShowNum(3,13,Time->tm_sec,2);
			
		}
		else//检验存储值的最高最低
		{	
			uint8_t Rec[5];
			uint8_t First=1;			//第一条记录直接当初始的 max/min，省掉"给个假初值"的麻烦
			float heat=0;
			float heatMax=0;
			float heatMin=0;
			float humidity=0;
			float humidityMax=0;
			float humidityMin=0;
			uint32_t Address=0x000000;
			for(int i=0;;i+=5)
			{	
				W25Q64_ReadData(Address+i,Rec);						//先读，再判
				if((Rec[0]&Rec[1]&Rec[2]&Rec[3]&Rec[4])==0xFF)		//5 个字节全 0xFF = 还没写过的格子，到底了
				{
					break;
				}
				heat=Rec[2]+0.1*Rec[3];
				humidity=Rec[0]+0.1*Rec[1];
				if(First)
				{
					heatMax=heat;
					heatMin=heat;
					humidityMax=humidity;
					humidityMin=humidity;
					First=0;
				}
				else
				{
					heatMax=math_Max(heatMax,heat);			
					heatMin=math_Min(heatMin,heat);
					humidityMax=math_Max(humidityMax,humidity);
					humidityMin=math_Min(humidityMin,humidity);
				}
				
			}
			if(First)											//一条记录都没读到
			{
				OLED_ShowString(4,1,"No Data");
			}

			/* 把统计结果填进空着的四个格子：湿度max/min、温度max/min */
			OLED_ShowNum(2,6,(uint16_t)humidityMax,2);
			OLED_ShowString(2,8,".");
			OLED_ShowNum(2,9,(uint16_t)(humidityMax*10)%10,1);

			OLED_ShowNum(2,11,(uint16_t)humidityMin,2);
			OLED_ShowString(2,13,".");
			OLED_ShowNum(2,14,(uint16_t)(humidityMin*10)%10,1);

			OLED_ShowNum(3,6,(uint16_t)heatMax,2);
			OLED_ShowString(3,8,".");
			OLED_ShowNum(3,9,(uint16_t)(heatMax*10)%10,1);

			OLED_ShowNum(3,11,(uint16_t)heatMin,2);
			OLED_ShowString(3,13,".");
			OLED_ShowNum(3,14,(uint16_t)(heatMin*10)%10,1);
		}

		/* ---------- 50ms 节拍 ---------- */
		Delay_ms(50);//就是两次温湿度取样要差1.5s
		Tick++;		//直接延迟会导致其他模式也延迟，节拍器比较好使
		if(Tick>=30)
		{
			Tick=0;
		}

		
	}
}
