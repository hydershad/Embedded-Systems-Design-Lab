//Lab8 Test Program
//tests simple adc IR input and wireless motor control using bluetooth control
//Hyder Shad
//Lab8
//11-9-18
//EE445L TTH 12:30-2pm Lab section

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

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include <stdint.h>
#include "PLL.h"
#include "UART.h"
#include "SysTickInts.h"
#include "motor.h"
#include "IRfollow.h"
char flag = 0;
char input = 0;
int mode = 1;
uint32_t adc = 0;
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}
//debug code
int main(void){							//Lab8 tests basic wireless motor control using HC-08 Bluetooth module connected to iphone bluetooth serial app
	
  PLL_Init(Bus50MHz);       // set system clock to 50 MHz
  UART_Init();             	// initialize UART5 on PE4-5 for 115200 baud rate
  OutCRLF();								//add Line Feed and Carrige Return to FIFO to initialize
  motor_init();							//set up Port D and F
	IR_Init();								//initialize adc for IR follower mode inputs
	SysTick_Init(200000);			//Systick Handler Checks for New Data
  while(1){
		if(flag){
			if(input == 'm'){
				mode = !mode;
			}
			if(mode){		//if in manual/RC mode interpret UART Commands for motor control
			
				if(input == 'd'){					//drive command
					drive();
					UART_OutChar(input);		//debug send back accepted command
				}
				if(input == 'b'){					//brake command, motors coast to halt
					brake();
					UART_OutChar(input);		//debug send back accepted command

				}
				if(input == 'r'){					//reverse command
					reverse();
					UART_OutChar(input);		//debug send back accepted command
				}
			flag = 0;										//reset flag for Systick
			}
			
			
			else{			//if in IR autonmatic steering/follow mode
				if(adc < 1200){
					drive();				//for testing purposes, just "turn" right
				}
				else if(adc>2900){
				  reverse();			//for testing purposes, just "turn" left
				}
				else{ brake();}		//for testing purposes, just "keep" straight
			}
		}
	}
}
