
//adc FIFO functions
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FIFO.h"


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

