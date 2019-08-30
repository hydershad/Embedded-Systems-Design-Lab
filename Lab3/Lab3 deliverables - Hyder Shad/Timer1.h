//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 3
//TTH 12:30 - 2pm
//9/28/2018
//Sound.h
//timer initializations and handler for the speaker output


#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

void Timer0A_Handler(void);
void Timer0A_Init(uint32_t period);
void Timer1_Init(void);
void Timer1A_Handler(void);
