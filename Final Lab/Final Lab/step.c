//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 11
//TTH 12:30 - 2pm
//12/5/2018
//step.c

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "step.h"
// PB3 connected to driver for stepper motor coil RED
// PB2 connected to driver for stepper motor coil BLUE
// PB1 connected to driver for stepper motor coil YELLOW
// PB0 connected to driver for stepper motor coil ORANGE
void Step_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x02;        // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02)==0); // allow time for clock to start
  
		GPIO_PORTB_LOCK_R = 0x4C4F434B;					  // 2) no need to unlock PB0-3
  GPIO_PORTB_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // 4) disable analog function on PB0-3
                                    // 5) no pullup for external switches
  GPIO_PORTB_DIR_R |= 0x0F;        	// 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x0F;         // 7) enable digital port
	stepper_pos = 0;
	stepper_state = 0;
}
		
int stepper_pos = 0;
int stepper_state = 0;
int step_direction = 1;		//1 = CW, 0 = CCW
void Step(void){			//scan back and forth for IR beam
	if(step_direction ==1){			
		switch(stepper_state){		//select which coils to toggle high and low to move it one step forward/backward
			case 0: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x0C;
						stepper_state = 1;
			break;
			case 1: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x06;
						stepper_state = 2;
			break;
			case 2:
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x03;
						stepper_state = 3;
			break;
			case 3: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x09;
						stepper_state = 0;
			break;
			default: break;
		}
		stepper_pos = stepper_pos +1;								//keep track of stepper position relative to car
		if(stepper_pos == 199){step_direction = 0;}	//continue scan in other direction
	}
	else{
				switch(stepper_state){
			case 0: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x0C;
						stepper_state = 3;
			break;
			case 1: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x06;
						stepper_state = 0;
			break;
			case 2:
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x03;
						stepper_state = 1;
			break;
			case 3: 
						GPIO_PORTB_DATA_R &= ~0x0F;
						GPIO_PORTB_DATA_R |= 0x09;
						stepper_state = 2;
			break;
			default: break;
		}
		stepper_pos = stepper_pos -1;
		if(stepper_pos == 0){step_direction = 1;}
	}
	

}


