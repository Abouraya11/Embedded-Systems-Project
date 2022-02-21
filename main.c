#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "DIO.h"
#include "bitwise.h"
#include "types.h"
#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"

// Handling Functions Prototypes
void PedBtnsHandler();
void Timer0AHandler();
void SysTickHandler();

// FSM State vars
static int traffic1State = 2010; // Traffic 1 State Variable, start state is RED off, GREEN on, YELLOW off Traffic 1
static int traffic2State = 2100; // Traffic 1 State Variable, start state is RED on, GREEN off, YELLOW off Traffic 1

// Needed Vars
static int timerCounter = 0, whichBtn = 0;

/*
    Main Function
*/
int main()
{
  // Enable and Check Clk
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    ;

  // Interrupt Priority Set
  NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x10; // Set Timer0A to Highest Priority
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x90; // Set SysTick to Lowest Priority
  IntPrioritySet(INT_GPIOF, 0x50);   // Set GPIOF to Middle Priority

  // PORT F INIT
  GPIO_PORTF_LOCK_R = 0x4C4F434B;                                                          // Unlock PortF
  GPIO_PORTF_CR_R = 0x01;                                                                  // Allow Changes
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);                                       // Set PIN_0 dir in
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);                                       // Set PIN_4 dir in
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);                                      // Set PIN_1 dir out
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);                                      // Set PIN_2 dir out
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);                                      // Set PIN_3 dir out
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // Set Pull Up PIN_0
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // Set Pull Up PIN_4

  // Allow Interrupts for GPIO
  GPIOIntRegister(GPIO_PORTF_BASE, PedBtnsHandler);
  GPIOIntEnable(GPIO_PORTF_BASE, (GPIO_PIN_4 | GPIO_PIN_0));

  // SysTic Init
  SysTickDisable();
  SysTickPeriodSet(15999999); // 1s - Repeated 5 times to count 5s GREEN or 2s YELLOW or 1s RED
  SysTickIntEnable();
  SysTickEnable();

  // Timer0A Init
  SYSCTL_RCGCTIMER_R |= 0x01; // Assign clk
  TIMER0_CTL_R = 0x00000000;  // Disable TimerA
  TIMER0_CFG_R = 0x00000000;  // Set TimerA to 32-bit
  TIMER0_TAMR_R = 0x00000002; // Set Periodic, Down
  TIMER0_IMR_R = 0x00000001;  // Enable interrupt in Timer0A
  NVIC_EN0_R = 1 << 19;       // Enable interrupts in NVIC

  // DIO INIT
  // Init Ground
  DIO_Init(PORTE, 3, OUT);
  DIO_WritePort(PORTE, 0x0);

  // Init Traffic 1 LEDs
  DIO_Init(PORTA, 2, OUT);
  DIO_Init(PORTA, 3, OUT);
  DIO_Init(PORTA, 4, OUT);

  // Init Traffic 2 LEDs
  DIO_Init(PORTC, 6, OUT);
  DIO_Init(PORTC, 4, OUT);
  DIO_Init(PORTC, 5, OUT);

  // Init Ped Traffic 1 LEDs
  DIO_Init(PORTD, 0, OUT);
  DIO_Init(PORTD, 1, OUT);

  // Init Ped Traffic 2 LEDs
  DIO_Init(PORTD, 2, OUT);
  DIO_Init(PORTD, 3, OUT);

  // In Circuit Matching - Electricity
  // Traffic Cars Start State
  DIO_WritePort(PORTC, 0x10); // Start Traffic 2 Cars RED
  // DIO_WritePort(PORTC, 0x0);  // Start Traffic 1 Cars Green
  DIO_WritePort(PORTA, 0x4); // Continue Starting Traffic 1 Cars Green
  // Traffic Ped Start State
  DIO_WritePort(PORTD, 0x6); // Traffic 1 Ped Red & Traffic 2 Ped Green

  /*
  Traffic 1 incircuit matching
  ----------------------------
  CARS
  ----
    Green -> Port A Pin 2
    Yellow ->  PortA Pin3
    Red ->  PortA Pin4
  PED
  ---
    Green -> Port D Pin 3
    Red -> Port D Pin 2

==================================

  Traffic 2 incircuit matching
  ----------------------------
  CARS
  ---
    Green -> Port C Pin 6
    Yellow -> Port C Pin 5
    Red -> Port C Pin 4
  PED
  ---
    Green -> Port D Pin 1
    Red -> Port D Pin 0
  */

  // Enable Interrupts for Processor
  __asm("CPSIE I");

  // Wait for Interrupt
  while (1)
  {
    __asm("  wfi\n");
  }
}

/*
    SysTick Handler Function, Input void, Output void
    Called when SysTick throws an Interrupt when its reaches zero
*/
void SysTickHandler()
{
  timerCounter++; // Increment SysTic counter to repeat either 5 times (GREEN), 2 times (YELLOW) or 1 time (RED) as a representation of seconds

  switch (traffic1State) // FSM Traffic 1
  {
  case 2010:               // state RED off, GREEN on, YELLOW off
    if (timerCounter == 5) // wait 5s for green light to elapse
    {
      timerCounter = 0;          // reset timerCounter
      DIO_WritePort(PORTA, 0x8); // Turn on YELLOW and Turn off RED & GREEN of Traffic 1
      traffic1State = 2001;      // go to state RED off, GREEN off, YELLOW on Traffic 1
    }
    break;

  case 2001:               // state RED off, GREEN off, YELLOW on
    if (timerCounter == 2) // wait 2s for green light to elapse
    {
      timerCounter = 0;           // reset timerCounter
      DIO_WritePort(PORTA, 0x10); // Turn on RED and Turn off YELLOW & GREEN of Traffic 1
      traffic1State = 2100;       // go to state RED on, GREEN off, YELLOW off Traffic 1
    }
    break;

  case 2100:               // state RED on, GREEN off, YELLOW off
    if (timerCounter == 1) // wait 1s for green light to elapse
    {
      timerCounter = 0;           // reset timerCounter
      DIO_WritePort(PORTC, 0x40); // Turn on GREEN and Turn off RED & YELLOW of Traffic 2
      DIO_WritePort(PORTD, 0x9);  // Turn on GREEN of Traffic 1 and Turn on RED of Traffic 2
      traffic1State = 2000;       // go to NONE state for Traffic 1
      traffic2State = 2010;       // go to state RED off, GREEN on, YELLOW off Traffic 2
    }
    break;
  }

  switch (traffic2State) // FSM Traffic 2
  {
  case 2010: // state RED off, GREEN on, YELLOW off
    if (timerCounter == 5)
    {
      timerCounter = 0;           // reset timerCounter
      DIO_WritePort(PORTC, 0x20); // Turn on YELLOW of Traffic 2 and Turn off RED & GREEN
      traffic2State = 2001;       // go to state  RED off, GREEN off, YELLOW Traffic 2
    }
    break;

  case 2001: // state RED off, GREEN off, YELLOW on
    if (timerCounter == 2)
    {
      timerCounter = 0;           // reset timerCounter
      DIO_WritePort(PORTC, 0x10); // Turn on YELLOW of Traffic 2 and Turn off RED & GREEN
      traffic2State = 2100;       // go to state RED on, GREEN off, YELLOW off Traffic 2
    }

    break;

  case 2100: // state RED on, GREEN off, YELLOW off
    if (timerCounter == 1)
    {
      timerCounter = 0;          // reset timerCounter
      DIO_WritePort(PORTA, 0x4); // Turn on GREEN and Turn off RED & YELLOW of Traffic 1
      DIO_WritePort(PORTD, 0x6); // Turn on RED of Ped Traffic 1 and Turn on Green of Ped Traffic 2
      traffic2State = 2000;      // go to NONE state for Traffic 2
      traffic1State = 2010;      // go to state RED off, GREEN on, YELLOW off Traffic 1
    }
    break;
  }
}

/*
    GPIO Port F Handler Function, Input void, Output void
    Called when SysTick throws an Interrupt either buttons are pressed
*/
void PedBtnsHandler()
{
  if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0)
  {
    whichBtn = 1; // Ped Button of Traffic 1 Ped Pressed
    if ((traffic1State == 2010) || (traffic1State == 2001))
    {
      TIMER0_CTL_R = 0x00000003;
      GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);

      TIMER0_CTL_R = 0x00;       // disable Timer0A
      TIMER0_TAILR_R = 31999998; // Set Reload of Timer0A to equivalent of 2s to wait for Ped to cross the Street.
      TIMER0_CTL_R = 0x03;       // enable Timer0A

      while (1)
      {
        SysTickDisable();           // Disable SysTick
        DIO_WritePort(PORTD, 0x0A); // Turn on RED of Traffic 1
        DIO_WritePort(PORTA, 0x10); // Turn on GREEN for both Ped Traffic 1,2 (Ped Traffic 2 is already GREEN, set Ped Traffic 1 to GREEN for 2s)
        __asm("  wfi\n");           // Wait for Interrupt of Timer0A before enabling SysTick
        SysTickEnable();            // Enable SysTick
        break;
      }
    }
  }
  if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
  {
    whichBtn = 2;                                           // Ped Button of Traffic 2 Ped Pressed
    if ((traffic2State == 2010) || (traffic2State == 2001)) // if GREEN or YELLOW
    {
      TIMER0_CTL_R = 0x00000003;
      GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); // Clear Interrupt Flag

      TIMER0_CTL_R = 0x00;       // disable Timer0A
      TIMER0_TAILR_R = 31999998; // Set Reload of Timer0A to equivalent of 2s to wait for Ped to cross the Street.
      TIMER0_CTL_R = 0x03;       // enable Timer0A

      while (1)
      {
        SysTickDisable();           // Disable SysTick
        DIO_WritePort(PORTC, 0x10); // Turn on RED of Traffic 2
        DIO_WritePort(PORTD, 0x0A); // Turn on GREEN for both Ped Traffic 1,2 (Ped Traffic 1 is already GREEN, set Ped Traffic 2 to GREEN for 2s)
        __asm("  wfi\n");           // Wait for Interrupt of Timer0A before enabling SysTick
        SysTickEnable();            // Enable SysTick
        break;
      }
    }
  }
}

/*
    Timer0AHandler Function, Input void, Output void
    Called when Timer0A throws an Interrupt when its reaches zero
*/
void Timer0AHandler()
{
  if (whichBtn == 2) // Ped pressed on Traffic 2
  {
    if (traffic2State == 2010) // RED off, GREEN on, YELLOW off
    {
      DIO_WritePort(PORTC, 0x40); // Turn on GREEN of Traffic 2
    }
    else if (traffic2State == 2001) // RED off, GREEN off, YELLOW on
    {
      DIO_WritePort(PORTC, 0x20); // Turn on YELLOW of Traffic 2
    }
    DIO_WritePort(PORTD, 0x09); // Turn on GREEN of Traffic 1 and RED of Traffic 2
  }

  if (whichBtn == 1) // Ped pressed on Traffic 1
  {
    if (traffic1State == 2010) // RED off, GREEN on, YELLOW off
    {
      DIO_WritePort(PORTA, 0x04); // Turn on GREEN of Traffic 1
    }
    else if (traffic1State == 2001) // RED off, GREEN off, YELLOW on
    {
      DIO_WritePort(PORTA, 0x08); // Turn on YELLOW of Traffic 1
    }
    DIO_WritePort(PORTD, 0x06); // Turn on GREEN of Traffic 2 and RED of Traffic 1
  }
  TIMER0_ICR_R = 0x00000001; // Clear Interrupt Flag
  TIMER0_CTL_R = 0x00000000; // Stop Timer
}
