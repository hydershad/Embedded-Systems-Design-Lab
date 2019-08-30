//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 11
//TTH 12:30 - 2pm
//12/5/2018

//motor.c
//motor and stepper control using l293NE H-bridge driver

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
int delay = 0;
//pd0-1 is left motor, pd6-7 is right
void motor_init(void){							//initialize port D and B as L293NE control inputs for motors and stepper
	SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate clock for Port D
  while((SYSCTL_PRGPIO_R&0x08)==0); // allow time for clock to start

	GPIO_PORTD_LOCK_R = 0x4C4F434B;// 2) no need to unlock PD0-1, 6-7
  GPIO_PORTD_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTD_AMSEL_R &= ~0xC3;      // 4) disable analog function on PD0-3
                                    // 5) no pullup for external switches
  GPIO_PORTD_DIR_R |= 0xC3;        	// 5) set direction to output
  GPIO_PORTD_AFSEL_R &= ~0xC3;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0xC3;         // 7) enable digital port

	SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02)==0); // allow time for clock to start
  
	GPIO_PORTB_LOCK_R = 0x4C4F434B;					  // 2) no need to unlock PB0-3
  GPIO_PORTB_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // 4) disable analog function on PB0-3
                                    // 5) no pullup for external switches
  GPIO_PORTB_DIR_R |= 0x0F;        	// 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x0F;         // 7) enable digital port
	
	SYSCTL_RCGCGPIO_R |= 0x20;				//portf leds for debug/heartbeat
  delay = SYSCTL_RCGCGPIO_R;// ready?
	GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x0C;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R |= 0x0E;    // 
  GPIO_PORTF_AFSEL_R &= ~0x0E; // disable alt func
  GPIO_PORTF_DEN_R |= 0x0E;			//
}


