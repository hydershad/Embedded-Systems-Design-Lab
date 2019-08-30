// ADCTimerTrigger.c
//Hyder Shad
//Lab9
//EE445L TTH 12:30pm-2pm Lab section
//11/16/1028



// Runs on TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Daniel Valvano
// August 6, 2015

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
 
// There are many choices to make when using the ADC, and many
// different combinations of settings will all do basically the
// same thing.  For simplicity, this function makes some choices
// for you.  When calling this function, be sure that it does
// not conflict with any other software that may be running on
// the microcontroller.  Particularly, ADC0 sample sequencer 3
// is used here because it only takes one sample, and only one
// sample is absolutely needed.  Sample sequencer 3 generates a
// raw interrupt when the conversion is complete, but it is not
// promoted to a controller interrupt.  Software triggers the
// ADC0 conversion and waits for the conversion to finish.  If
// somewhat precise periodic measurements are required, the
// software trigger can occur in a periodic interrupt.  This
// approach has the advantage of being simple.  However, it does
// not guarantee real-time.
//
// A better approach would be to use a hardware timer to trigger
// the ADC0 conversion independently from software and generate
// an interrupt when the conversion is finished.  Then, the
// software can transfer the conversion result to memory and
// process it after all measurements are complete.



// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain9 (PE4)
// SS3 interrupts: enabled but not promoted to controller
 
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADCTimerTrigger.h"

//FIFO variables

int fifo[FIFO_SIZE];
int fifoStart = 0;
int fifoEnd = 0;


void ADC0_Init(void){
	volatile uint32_t delay;
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10;    // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) != 0x10); // wait for clock on port
  GPIO_PORTE_DIR_R &= ~0x08;      // 2) make PE3 input
  GPIO_PORTE_AFSEL_R |= 0x08;     // 3) enable alternate function on PE3
  GPIO_PORTE_DEN_R &= ~0x08;      // 4) disable digital I/O on PE3
  GPIO_PORTE_AMSEL_R |= 0x08;     // 5) enable analog functionality on PE3

	ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority

  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R = (ADC0_EMUX_R & ~0x0F000) | 0x05000;  // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  //ADC0_SSMUX3_R += 0;           // set channel to 0
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R |= 0x0008;						// 13) disable SS3 interrupts
  ADC0_SAC_R |= 0x6;              // enable x64 sampling
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
	
	NVIC_PRI4_R = (NVIC_PRI4_R & ~0x0FF00) | 0x04000; // timer0 priority 2
	NVIC_EN0_R = 1 << 17;						// enable interrupt 17 in NVIC
	
	// "zero" out with bad data
	for(int i = 0; i < FIFO_SIZE; i += 1) {
		fifo[i] = -1;
	}
}

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
void ADC0Seq3_Handler(void){
	ADC0_ISC_R = 0x0008;
	uint32_t result = ADC0_SSFIFO3_R&0xFFF;
	if((fifoStart + 1) % FIFO_SIZE == fifoEnd) {
		adc_fifo_Pop();
	}
	adc_fifo_Push(result);
}


//ADC FIFO low level implementation
//FIFO Functions


void adc_fifo_Push(int data) {
	int nextFifoEnd = (fifoEnd + 1) % FIFO_SIZE;
	if(nextFifoEnd == fifoStart) {
		return;
	}
	fifoEnd = nextFifoEnd;
	fifo[fifoEnd] = data;
}

int adc_fifo_Pop(void) {
	if(fifoStart == fifoEnd) {
		return -1;
	}
	int poppedValue = fifo[fifoStart];
	fifoStart = (fifoStart + 1) % FIFO_SIZE;
	return poppedValue;
}


int* adc_fifo_Get(void) {
	return fifo;
}

int adc_fifo_Current(void) {
	return fifo[fifoStart];
}



