#ifndef __W25Q64_H
#define __W25Q64_H

void W25Q64_Init(void);
void W25Q64_WriteENABLE(void);
void W25Q64_BusyCheck(void);
void W25Q64_Programming(uint32_t Address,uint8_t Data[],uint16_t Len);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_ReadData(uint32_t Address,uint8_t Rec[]);

#endif
