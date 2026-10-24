#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "W25Q64_ins.h"

void W25Q64_Init(void)
{
	MySPI_Init();
}
void W25Q64_WriteENABLE(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_StOP();
}
void W25Q64_BusyCheck(void)
{	
	uint32_t Timeout = 0;
	W25Q64_WriteENABLE();
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);//读busy位
	while(((MySPI_SwapByte(W25Q64_DUMMY_BYTE)&0x01)==0x01) && (Timeout < 100000))	//约1.8秒超时
	{
		Timeout++;
	}
	MySPI_StOP();

}
/* 按页自动拆分：一次 Page Program 不能跨页（256 字节），
   跨了会在页内回卷、把本页开头的数据打掉。
   所以这里按"本页还剩几个字节"切刀，一刀一刀写完。 */
void W25Q64_Programming(uint32_t Address,uint8_t Data[],uint16_t Len)//**************************************
{
	uint16_t i = 0;		//已经写出去的字节数，同时也是 Data[] 下一个要发的下标
	uint16_t Room;		//本页从 Address 起还剩几个字节可写：1~256（Address 正好在页首时是 256）
	uint16_t N;			//这一刀写几个字节 = min(Len-i, Room)；写完 Address 和 i 各加 N
	while(i < Len)
	{
		Room = 256 - (Address % 256);				//本页从 Address 起还能写多少字节
		N = ((Len - i) < Room) ? (Len - i) : Room;	//这一刀切几个
		W25Q64_WriteENABLE();
		MySPI_Start();
		MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
		MySPI_SwapByte(Address>>16);
		MySPI_SwapByte(Address>>8);
		MySPI_SwapByte(Address);
		for(int k=0;k<N;k++)
		{
			MySPI_SwapByte(Data[i+k]);	//从第 i 个字节接着发，不是每次都从 Data[0]
		}
		MySPI_StOP();
		W25Q64_BusyCheck();			//每一刀都要等它写完
		Address += N;
		i += N;
	}
}
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteENABLE();
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8);
	MySPI_SwapByte(Address);
	MySPI_StOP();
	W25Q64_BusyCheck();

}
void W25Q64_ReadData(uint32_t Address,uint8_t Rec[5])
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address>>16);
	MySPI_SwapByte(Address>>8);
	MySPI_SwapByte(Address);
	for(int i=0;i<5;i++)
	{
		Rec[i]=MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	
	}
	MySPI_StOP();
}
