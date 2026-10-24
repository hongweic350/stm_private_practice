#ifndef __DHT11_H
#define __DHT11_H

void DHT11_Init(void);
uint8_t DHT11_ReadData(uint8_t data[5]);

#endif
