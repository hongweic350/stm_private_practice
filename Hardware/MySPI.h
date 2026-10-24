#ifndef __MYSPI_H
#define __MYSPI_H

void MySPI_SS(uint8_t val);
void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_StOP(void);
uint8_t MySPI_SwapByte(uint8_t Data);

#endif
