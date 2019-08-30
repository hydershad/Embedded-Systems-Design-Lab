PI.c

#include <stdint.h>
#include "pwm.h"
#include "tm4c123gh6pm.h"

extern uint32_t Target_Speed;
extern uint32_t Period;     // 24-bit, 12.5 ns units
uint32_t Speed;      				//motor speed in 0.1 rps
int32_t Error;           		// speed error in 0.1 rps
int32_t Duty;           		// duty cycle 40 to 39960

void Timer0A_Handler(void){
	
	TIMER0_ICR_R = 0x01;      			// acknowledge timer2A
  Speed = 200000000/Period; 			// 0.1 rps
  Error = Target_Speed-Speed;     // 0.1 rps
  Duty = Duty+(3*E)/64;           // discrete integral
  if(Duty < 100) Duty=100;        // Constrain output
  if(Duty>39900) Duty=39900;      // 100 to 39900
  PWM_Duty(Duty);            		// output
}

