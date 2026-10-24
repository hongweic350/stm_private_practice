#include "stm32f10x.h"       // Device header
#include "Wire-1.h"

void DHT11_Init(void)
{
	Wire_Init();

}
uint8_t DHT11_ReadData(uint8_t data[5])
{
	uint8_t ack;
	Wire_Start();
	ack= Wire_IO(data);
	return ack;
}
