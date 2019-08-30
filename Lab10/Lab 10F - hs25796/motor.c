//motor.c
//motor control using l293NE H-bridge driver

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
int delay = 0;

/* do not use
#define PE0       (*((volatile uint32_t *)0x40024004))    // RDY from ESP 8266
#define PE1       (*((volatile uint32_t *)0x40024008))    // RST_B to ESP8266
#define PE3       (*((volatile uint32_t *)0x40024020))    // LED to indicate that the 8266 connected
  */
void motor_init(void){		//initialize fort f ad indicator leds and port d as L293NE motor control inputs
	SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate clock for Port D
  while((SYSCTL_PRGPIO_R&0x08)==0); // allow time for clock to start
                                    // 2) no need to unlock PD0-3
  GPIO_PORTD_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 4) disable analog function on PD0-3
                                    // 5) no pullup for external switches
  GPIO_PORTD_DIR_R |= 0x0F;        	// 5) set direction to output
  GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital port
	
	SYSCTL_RCGCGPIO_R |= 0x20;         		// activate port F
	while((SYSCTL_PRGPIO_R&0x20)==0); 		// allow time for clock to start
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
}

void brake(void){		//kill motor power, allow coasting, PD1-2 LOW
	GPIO_PORTF_DATA_R &= ~0x06;
	GPIO_PORTD_DATA_R &= ~0x03;
}

void drive(void){		//forward drive, set ONLY PD1 HIGH
	GPIO_PORTF_DATA_R &= ~0x06;
	GPIO_PORTF_DATA_R |= 0x02;
		GPIO_PORTD_DATA_R &= ~0x07;
	GPIO_PORTD_DATA_R |= 0x02;
}
void reverse(void){	//reverse drive, set ONLY PD2 HIGH
		GPIO_PORTF_DATA_R &= ~0x06;
	  GPIO_PORTF_DATA_R |= 0x04;
			GPIO_PORTD_DATA_R &= ~0x07;
	  GPIO_PORTD_DATA_R |= 0x04;
}

