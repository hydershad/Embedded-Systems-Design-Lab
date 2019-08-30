//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 11
//TTH 12:30 - 2pm
//12/5/2018
//step.h

#include <stdint.h>
#include "tm4c123gh6pm.h"
extern int stepper_state;
extern int stepper_pos;	
extern int step_direction;
void Step_Init(void);
void Step(void);
