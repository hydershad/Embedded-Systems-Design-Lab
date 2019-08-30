//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 3
//TTH 12:30 - 2pm
//9/28/2018
//switch.c
//button read and debounce, port intitialization for PE0-3

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "Switch.h"
extern int reset;
extern uint16_t hour;						// holds the current hour
extern uint16_t minute;						// holds the current minute
extern uint16_t second;
extern uint16_t am_pm;
extern uint16_t alarm_am_pm;
extern uint16_t alarm;					//is alarm on
extern uint16_t a_hour;					//keep track of alarm time
extern uint16_t a_minute;
extern uint16_t set_alarm;			//is button pressed for alarm time edit
extern uint16_t buttonpress;
void Buttons_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)==0); // allow time for clock to start
                                    // 2) no need to unlock PE0-3
  GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 4) disable analog function on PE0-3
                                    // 5) no pullup for external switches
  GPIO_PORTE_DIR_R &= ~0x0F;        // 5) set direction to output
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 6) regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable digital port
	
}


// bool debounce[4] = { hours, minutes, arm_alarm, set_alarm };
int debounce[4];


void Buttons_ReadInput(void) {										
	int32_t data = GPIO_PORTE_DATA_R;						
	if((data & 0x01) != 0)					
		debounce[0] = (data & 0x01)*5;						//debounce arrays, holds value of button presses and rechecks
	if((data & 0x02) != 0)
		debounce[1] = ((data & 0x2) >> 1)*5;		
	if((data & 0x04) != 0)
		debounce[2] = ((data & 0x4) >> 2)*5;
	if((data & 0x08) != 0)
		debounce[3] = ((data & 0x8) >> 3)*5;
}

void Buttons_Pressed(uint32_t button) {
	buttonpress = 1;
	if(button == 2){								//if button 2 pressed, change alarm status
				alarm = ! alarm;
			}
  if(button == 3){								//if button 3 pressed, change mode to alarm setup screen or regular screen
				set_alarm = ! set_alarm;
				reset = 1;								//trigger reset of the screen
		}
	if(! set_alarm) {							//clock time if not in alarm display screen
		if(button == 0) {
			if(hour == 11){
				am_pm = !am_pm;
				hour = hour +1;
			}
			else if(hour == 12){
				hour = 1;
			}
			else{hour = hour + 1;}
		}
		else if(button == 1) {
			if(minute+1 == 60) {
				if(hour == 11){
					am_pm = !am_pm;
					hour = hour +1;
					minute = 0;
				}
				else if(hour == 12){
					hour = 1;
					minute = 0;
				}
				else{
					hour = hour + 1;
					minute = 0;
				}
			}
			else{minute = minute + 1;}
		}
		second = 0;
	}
	else {																	//is set alarm active, then change alarm time
				if(button == 0){
					if(a_hour ==11){
						alarm_am_pm = !alarm_am_pm;
						a_hour = a_hour +1;
					}
					else if(a_hour == 12){
						a_hour = 1;
					}
					else{a_hour = a_hour + 1;}
				}
		 else if(button == 1){
						if(a_minute+1 == 60){
							if(a_hour == 11){
								alarm_am_pm = !alarm_am_pm;
								a_hour = a_hour + 1;
								a_minute = 0;
							}
							else if(a_hour == 12){
								a_hour = 1;
								a_minute = 0;
							}
							else{
							a_hour = a_hour + 1;
							a_minute = 0;
							}
						}
					else{a_minute = a_minute + 1;}	
		 }
		 
}
}

void Buttons_10ms_Handler(void) {							//read gpio register values to see if buttons were pressed
	Buttons_ReadInput();
	for(uint32_t i = 0; i < 4; i += 1) {				//debounce each button
		if(debounce[i] > 0) {
			debounce[i] -= 1;
			if(debounce[i] == 0) {
				Buttons_Pressed(i);										//implement logic for alarm/time/display based on button presses
			}
		}
	}
}

