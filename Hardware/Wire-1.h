#ifndef	__WIRE_H
#define __WIRE_H

void Wire_Init(void);
void Wire_Out(void);
void Wire_Start(void);
uint8_t Wire_Check(void);
uint8_t Wire_IO(uint8_t data[5]);

#endif
