//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 11
//TTH 12:30 - 2pm
//12/5/2018

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 5.11 Section 5.6, Program 3.10

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */



#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "PLL.h"
#include "SysTickInts.h"
#include "motor.h"

#include "ST7735.h"
#include "ADCT0ATrigger.h"
#include "UART5.h"
#include "step.h"
#include "PWM.h"
#include "ports.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
char buffer [64] = {0};				//UART buffer
char flag = 0;								//motor control flag
char input = 0;
int alt_drive = 0;						//alternate between driving and steering wheels
uint32_t adc = 0;
uint32_t ADC0_InSeq3(void);
int fifostatus = 0;
char ax[8]  = "0";   //string arrays to convert data values
char ay[8]  = "0";    
char az[8]  = "0"; 
int x_val, y_val, z_val = 0;			//integer gyro/accel values
int main(void){							
	
  PLL_Init(Bus80MHz);       // set system clock to 50 MHz
	DisableInterrupts();
		Step_Init();		//12v, oscillating rotation 
  motor_init();							//set up Port D and F
	Output_Init();						//st7735 redtab initialization
	UART5_start();
	SysTick_Init(400000);			//Systick Handler SETS FLAG TO CHECK for New Data

	PWM0B_Init(40000,20000);
	ADC0_InitSoftwareTriggerSeq3PD3();
	EnableInterrupts();
	GPIO_PORTD_DATA_R &= ~0xC3;
	
  while(1){

	if(flag){
		//check for UART data in FIFO
		if(ESP8266_GetMessage(buffer)){			//input recieved as a string, values separated by ;
		strcpy(ax, strtok(buffer, ";"));
		strcpy(ay, strtok(NULL, ";"));
		strcpy(az, strtok(NULL, ";"));	
    //strcpy(&pin_int, buffer);       // Integer value that is determined by the Blynk App
    x_val = atoi(ax);     // Need to convert ASCII to integer
		y_val = atoi(ay);
		z_val = atoi(az);		//not used for motion, just for debug
		
	if(alt_drive){	
			
		if(-5350 <x_val < -1200){
			PWM0B_Duty(20000);			//half power reverse
			GPIO_PORTD_DATA_R &= ~0xC3;
			GPIO_PORTD_DATA_R |= 0x41;
		}
		else if(x_val < -5350){
			PWM0B_Duty(40000);		//full power reverse
			GPIO_PORTD_DATA_R &= ~0xC3;
			GPIO_PORTD_DATA_R |= 0x41;
		}

		else if(3800 <x_val < 6600){
			PWM0B_Duty(20000);						//half power drive
			GPIO_PORTD_DATA_R &= ~0xC3;
			GPIO_PORTD_DATA_R |= 0x82;
		}
		else if(6600 <x_val){
			PWM0B_Duty(40000);						//full power drive
			GPIO_PORTD_DATA_R &= ~0xC3;
			GPIO_PORTD_DATA_R |= 0x82;
		}
		else{
				GPIO_PORTD_DATA_R = 0x00;		//coast
		}
	}

	else{
		if(y_val>6200){									//left turn
			alt_drive = !alt_drive;
				GPIO_PORTD_DATA_R &= ~0xC3;
				GPIO_PORTD_DATA_R |= 0x40;
		}
		else if(y_val<-4700){						//right turn
			GPIO_PORTD_DATA_R &= ~0xC3;
			GPIO_PORTD_DATA_R |= 0x01;
		}
			else{
				GPIO_PORTD_DATA_R = 0x00;		//coast;
		}
	}
	Output_Clear();				//output UART info on screen for debug and user viewing
	ST7735_SetCursor(0,0);
		ST7735_OutString("ax = ");			//accelerometer info
	ST7735_OutString(ax);
	ST7735_SetCursor(0,1);
			ST7735_OutString("ay = ");

	ST7735_OutString(ay);
	ST7735_SetCursor(0,2);
			ST7735_OutString("az = ");

	ST7735_OutString(az);
		ST7735_SetCursor(0,4);
	ST7735_OutUDec(x_val);
		ST7735_SetCursor(0,5);
			ST7735_OutString("lidar position = ");		//stepper motor position
	ST7735_OutUDec(stepper_pos);
			
	}
	alt_drive = !alt_drive;		//toggle between turning commands and driving commands so robot moves in nice arc
	flag= 0;									//reset flag so code will run after SysTick Interrupt
	}
 }
}
void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x0C;		//heartbeat for debug
flag = 1;												//set flag for motor control
	uint32_t x = ADC0_InSeq3();		//scan for IR beam
	if(x<50){
				Step();
	}
}

uint32_t ADC0_InSeq3(void){  uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}
