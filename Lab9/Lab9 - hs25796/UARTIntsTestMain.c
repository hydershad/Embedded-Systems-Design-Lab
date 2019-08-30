//main.c
//Hyder Shad
//Lab9
//EE445L TTH 12:30pm-2pm Lab section
//11/16/1028

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



#include <stdio.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include "PLL.h"
#include "ADCTimerTrigger.h"
#include "calibration.h"

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none

//debug code
void OutCRLF(void){			//load UART with linefeed + carrige return
  UART_OutChar(CR);
  UART_OutChar(LF);
}

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern uint32_t currentIndex;
extern uint32_t data[100];

int currentTemp = 0;	//current temperature reading
int pixel = 0;				//used for drawing temperature reading plot

int main(void){ 

	DisableInterrupts();
	PLL_Init(Bus80MHz);						// bus clock at 80 MHz
	UART_Init();									//initialize UART to Computer
	ST7735_InitR(INITR_REDTAB);		//initialize lcd
	ADC0_Init();									//start ADC and initialize ADC FIFO
	EnableInterrupts();
	
	while(1) {
		ST7735_SetCursor(0, 0);
		currentTemp = realTemp(adc_fifo_Current());
		printf("Current Temperature: %d.%02d\n", (currentTemp / 100), (currentTemp % 100));		//send out temperature reading
		ST7735_PlotClear(32, 159);					//clear plot area for next measurements display

		
		for(int i = 0; i < FIFO_SIZE; i += 1) {					
			pixel = 128 - (realTemp(adc_fifo_Get()[i]) * 2 / 100);			//fit data to plot
			ST7735_DrawPixel(i+1, pixel+1, ST7735_CYAN);
			ST7735_DrawPixel(i+1, pixel, ST7735_CYAN);
			ST7735_DrawPixel(i, pixel+1, ST7735_CYAN);
			ST7735_DrawPixel(i, pixel, ST7735_CYAN);
		}
		WaitForInterrupt();					//low power wait
	}
}
