
 //Hyder Shad
 //UT EID: hs25796
 //Lab 2 
 //main.c file
 //9/20/18
 //EE445L Fall 2018, TTH 12:30pm - 2pm  Lab



/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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
 


// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdio.h>
#include "ADCSWTrigger.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Timer1.h"
#include "string.h"
#include "math.h"

#define PF2         (*((volatile uint32_t *)0x40025010))
#define PF1         (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void Timer1_Init(void);				//init timer1 for 12.5ns intervals
void Draw_Line(int sX, int sY, int eX, int eY);

//globals

int time[1000] = {0};		//sampling timestamps
int adcPoints[1000] = {0};		//ADC meaasurements
int arr_index = 0;			//array index for time/adc
int range = 0;			//
uint32_t mintime = 0xFFFFFFFF; //set up mintime and maxtime to calculate jitter
uint32_t maxtime = 0;
uint32_t minADC = 0xFFFFFFFF;
uint32_t maxADC = 0;
int graph[4096] = {10}; 	//check to make sure index is correct for adc
int jitter = 0;

//////////

volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
	delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;                   // profile
	
		//data collection, 1000 points using avrg
	
    if((arr_index) < 1000) {
    time[arr_index] = TIMER1_TAR_R;
    adcPoints[arr_index] = ADCvalue;
    arr_index  = arr_index + 1;
  }
}
int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	arr_index = 0;
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer1_Init();
	Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
	 
  EnableInterrupts();
    
	ST7735_InitR(INITR_REDTAB);		//initialize display
	PF2 = 0;                      // turn off LED
	PF1 = 0;
  Timer1_Init();
	while(arr_index < 1000){			//during data collection, toggle board led

	//PF1 = PF1^0x2;
	PF1 = (PF1*12345678)/1234567+0x02;  // this line causes jitter
	}
	DisableInterrupts();					//disable trigger for adc, stop measuring values after 1000 captures
	
	for(int i = 0; i < 999; i = i+1) {		//find minimum delta and max delta times for sampling to calculate jitter
		range = (time[i+1] - time[i]);			//find diference in time stamps to calculate jitter
	if(range<0){range = -range;}					//make sure difference is absolute value
		if(range < mintime) {								//update minimum time
			mintime = range;
		}
		if(range > maxtime){								//update maximum time			
			maxtime = range;
		}
	
}
 jitter = maxtime - mintime;  			//calculate jitter time, should be <10ms

	for(int i = 1; i < 1000; i = i+1) {			//create histogram with # of occurences for each ADC Value
		graph[adcPoints[i]] = graph[adcPoints[i]] + 1;
		if(adcPoints[i] < minADC) {						//record minimum ADC value
			minADC = adcPoints[i];
		}
		else if(adcPoints[i] > maxADC) {			//record max ADC value
			maxADC = adcPoints[i];
		}
	} 
	
	ST7735_SetCursor(0,0);									//produce PMF plot
	ST7735_PlotClear(0,700);
	for(int z = 0; z<4096; z = z +1){				//for each ADC possible value draw the number of occurences as a vertical bar
	ST7735_PlotBar(graph[z]);
	ST7735_PlotNext();												
	}
	ST7735_OutString("Min ADC value: ");		//output ADC min and max Values
  ST7735_OutUDec( minADC);
	ST7735_SetCursor(0,1);
	ST7735_OutString("Max ADC value: ");		
	ST7735_OutUDec( maxADC);
	ST7735_SetCursor(0,2);
	ST7735_OutString("Jitter: ");					//output calculated Jitter value
	ST7735_OutUDec(jitter); 
	
	//Draw_Line(10, 10, 50, 50);

}

void Draw_Line(int startX, int startY, int endX, int endY) {		//DrawLine function for lab 3
	
	for(int i = startX; i < endX; i = i + 1) {		
		ST7735_DrawPixel(i, startY, ST7735_CYAN);
	}
	for(int16_t j = startY; j < endY; j = j + 1) {
		ST7735_DrawPixel(startX, j, ST7735_CYAN);
	}
}

