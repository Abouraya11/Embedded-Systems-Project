#include "types.h"

#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3
#define PORTE 4
#define PORTF 5

#define IN 0
#define OUT 1

void DIO_Init( int8 port, int8 bit, int8 dir);
void DIO_WritePin( int8 port, int8 bit, int8 value);
void DIO_WritePort( int8 port, int32 value);
int8 DIO_ReadPin (int8 Port, int8 pin);
int32 DIO_Read_Port (int8 Port);
