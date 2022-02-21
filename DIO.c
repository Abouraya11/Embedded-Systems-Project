#include "DIO.h"
#include "bitwise.h"
#include "types.h"
#include "stdint.h"
#include "tm4c123gh6pm.h"

void DIO_Init( int8 port, int8 bit, int8 dir){
  Set_Bit(SYSCTL_RCGCGPIO_R, port)
  while((SYSCTL_PRGPIO_R&(1<<port)) == 0){}
  
  switch(port)
  {
  case PORTA:
    {
    GPIO_PORTA_LOCK_R = 0x4C4F434B;
    Set_Bit(GPIO_PORTA_CR_R,bit)
      if(dir)
        Set_Bit(GPIO_PORTA_DIR_R, bit)
      else
        Clear_Bit(GPIO_PORTA_DIR_R, bit)
      Set_Bit(GPIO_PORTA_PUR_R,bit)
      Set_Bit(GPIO_PORTA_DEN_R,bit)
      break;
    }
  case PORTB:
    {
    GPIO_PORTB_LOCK_R = 0x4C4F434B;
    Set_Bit(GPIO_PORTB_CR_R,bit)
      if(dir)
        Set_Bit(GPIO_PORTB_DIR_R, bit)
      else
        Clear_Bit(GPIO_PORTB_DIR_R, bit)
      Set_Bit(GPIO_PORTB_PUR_R,bit)
      Set_Bit(GPIO_PORTB_DEN_R,bit)
      break;
    }
  case PORTC:
    {
 
    Set_Bit(GPIO_PORTC_CR_R,bit)
      if(dir)
        Set_Bit(GPIO_PORTC_DIR_R, bit)
      else
        Clear_Bit(GPIO_PORTC_DIR_R, bit)
      Set_Bit(GPIO_PORTC_PUR_R,bit)
      Set_Bit(GPIO_PORTC_DEN_R,bit)
      break;
    }
  case PORTD:
    {
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    Set_Bit(GPIO_PORTD_CR_R,bit)
      if(dir)
        Set_Bit(GPIO_PORTD_DIR_R, bit)
      else
        Clear_Bit(GPIO_PORTD_DIR_R, bit)
      Set_Bit(GPIO_PORTD_PUR_R,bit)
      Set_Bit(GPIO_PORTD_DEN_R,bit)
      break;
    }
  case PORTE:
    {

    Set_Bit(GPIO_PORTE_CR_R,bit)
      if(dir){
        Set_Bit(GPIO_PORTE_DIR_R, bit)
        Set_Bit(GPIO_PORTE_ODR_R , bit)
      }
      else
        Clear_Bit(GPIO_PORTE_DIR_R, bit)
      Set_Bit(GPIO_PORTE_PUR_R,bit)
      Set_Bit(GPIO_PORTE_DEN_R,bit)
      break;
    }
  case PORTF:
    {
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    Set_Bit(GPIO_PORTF_CR_R,bit)
      if(dir)
        Set_Bit(GPIO_PORTF_DIR_R, bit)
      else
        Clear_Bit(GPIO_PORTF_DIR_R, bit)
      Set_Bit(GPIO_PORTF_PUR_R,bit)
      Set_Bit(GPIO_PORTF_DEN_R,bit)
      break;
    }
  }
}

void DIO_WritePin( int8 port, int8 bit, int8 value){
  switch(port){
  case PORTA:
      if(value)
        Set_Bit(GPIO_PORTA_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTA_DATA_R, bit)
      break;
  case PORTB:
      if(value)
        Set_Bit(GPIO_PORTB_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTB_DATA_R, bit)
      break;
  case PORTC:
      if(value)
        Set_Bit(GPIO_PORTC_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTC_DATA_R, bit)
      break;
  case PORTD:
      if(value)
        Set_Bit(GPIO_PORTD_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTD_DATA_R, bit)
      break;
  case PORTE:
      if(value)
        Set_Bit(GPIO_PORTE_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTE_DATA_R, bit)
      break;
  case PORTF:
      if(value)
        Set_Bit(GPIO_PORTF_DATA_R, bit)
      else
        Clear_Bit(GPIO_PORTF_DATA_R, bit)
      break;
  }
}

void DIO_WritePort( int8 port, int32 value){
  switch(port){
  case PORTA:
      GPIO_PORTA_DATA_R=value;
      break;
  case PORTB:
      GPIO_PORTB_DATA_R=value;
      break;
  case PORTC:
      GPIO_PORTC_DATA_R=value;
      break;
  case PORTD:
      GPIO_PORTD_DATA_R=value;
      break;
  case PORTE:
      GPIO_PORTE_DATA_R=value;
      break;
  case PORTF:
      GPIO_PORTF_DATA_R=value;
      break;
  }
}
int8 DIO_ReadPin (int8 port, int8 pin){
  int8 bit;
  switch(port){
  case PORTA:
    {
      bit=Get_Bit(GPIO_PORTA_DATA_R,pin);
      break;
    }
  case PORTB:
    {
      bit=Get_Bit(GPIO_PORTB_DATA_R,pin);
      break;
    }
  case PORTC:
    {
      bit=Get_Bit(GPIO_PORTC_DATA_R,pin);
      break;
    }
  case PORTD:
    {
      bit=Get_Bit(GPIO_PORTD_DATA_R,pin);
      break;
    }
  case PORTE:
    {
      bit=Get_Bit(GPIO_PORTE_DATA_R,pin);
      break;
    }
  case PORTF:
    {
      bit=Get_Bit(GPIO_PORTF_DATA_R,pin);
      break;
    }
  }
  return bit;
}
int32 DIO_Read_Port (int8 port){
  switch(port){
  case PORTA:
    return GPIO_PORTA_DATA_R;
    break;
  case PORTB:
    return GPIO_PORTB_DATA_R;
    break;
  case PORTC:
    return GPIO_PORTC_DATA_R;
    break;
  case PORTD:
    return GPIO_PORTD_DATA_R;
    break;
  case PORTE:
    return GPIO_PORTE_DATA_R;
    break;
  case PORTF:
    return GPIO_PORTF_DATA_R;
    break;
  default: return 0;
  }
  
}

