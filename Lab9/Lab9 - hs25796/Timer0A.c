//timer0A.c
//Hyder Shad
//Lab9
//EE445L TTH 12:30pm-2pm Lab section
//11/16/1028

// timer0A setup
#include <stdint.h>
#include "tm4c123gh6pm.h"

int delay = 0;
void Timer0A_Init(void){
	SYSCTL_RCGCTIMER_R |= 0x01;			// enable timer0
	delay = SYSCTL_RCGCGPIO_R;			// delay
	TIMER0_CTL_R = 0x0;							// disable timer
	TIMER0_CTL_R |= 0x020;					// enable timer0 to trigger ADC
	TIMER0_CFG_R = 0;								// configure for 32-bit timer mode
	TIMER0_TAMR_R = 0x02;						// configure for periodic mode
	TIMER0_TAPR_R = 0;							// prescale value for trigger
	TIMER0_TAILR_R = 0x1387F;				// reload time (period)
	TIMER0_IMR_R = 0x0;							// disable all interrupts
	TIMER0_CTL_R |= 0x01;						// enable timer0A 32-b, periodic
	
}

