//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 3
//TTH 12:30 - 2pm
//9/28/2018
//Lab3main.c
//keeps track of time, controls clock interface, alarm settings, DrawPixel line function

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

   Program 5.12, section 5.7

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

// oscilloscope or LED connected to PF2 for period measurement
#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "SysTickInts.h"
#include "PLL.h"
#include "string.h"
#include "Switch.h"
#include "Timer1.h"
#include "HandPositions.h"			//holds arrays for analog clock face image coordintes on screen
#include "ST7735.h"

#define PF2     (*((volatile uint32_t *)0x40025010))			//heartbeat for 1s time elapsed

//call functions
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void DrawLine(int startX, int startY, int endX, int endY, int color);

//global variables for use across functions
int reset = 0;
uint16_t hour = 2;					//keep track of time
uint16_t minute = 50;
uint16_t second = 0;
uint16_t a_hour = 2;					//keep track of time
uint16_t a_minute = 52;
uint16_t am_pm = 0;	
uint16_t alarm_am_pm = 0;

uint16_t ring = 0;
uint16_t alarm = 0;					//alarm on/off
uint16_t set_alarm = 0;					//set up alarm button status
uint16_t flag = 0; 					//flag for interrupt handler to set when 1s elasped
uint16_t ahx = 0;					//alarm display hand positions
uint16_t ahy = 0;
uint16_t amx = 0;
uint16_t amy = 0;
uint16_t old_ahx = 0;
uint16_t old_ahy = 0;
uint16_t old_amx = 0;
uint16_t old_amy = 0;



uint16_t old_secX = 0;			//variables for position of hands for redrawing instead of reinitializing screen
uint16_t old_secY = 0;
uint16_t current_secX = 0;
uint16_t current_secY = 0;
uint16_t old_minX = 0;
uint16_t old_minY = 0;
uint16_t current_minX = 0;
uint16_t current_minY = 0;
uint16_t old_hourX = 0;
uint16_t old_hourY = 0;
uint16_t current_hourX = 0;
uint16_t current_hourY = 0;
//////////
uint16_t buttonpress = 0;
//arrays for second/miinute hand positions
uint16_t secX_array [60]  = {62, 65, 69, 72, 75, 77, 82, 85, 87, 86, 86, 91, 93, 95, 95, 95, 94, 96, 96, 94, 91, 90, 89, 86, 83, 77, 74, 73, 70, 66, 62, 58, 55, 51, 48, 42, 40, 37, 36, 36, 33, 30, 28, 30, 31, 31, 30, 30, 31, 34, 35, 36, 39, 43, 48, 50, 52, 55, 56, 57};	
uint16_t secY_array [60]  = {37, 35, 34, 34, 34, 38, 39, 40, 40, 45, 50, 54, 57, 62, 66, 71, 75, 78, 81, 83, 86, 92, 95, 97, 99, 100, 101, 104, 105, 106, 105, 106, 107, 107, 103, 100, 99, 98, 95, 90, 86, 83, 82, 81, 76, 71, 67, 64, 61, 58, 55, 52, 48, 44, 43, 42, 40, 38, 37, 36};

uint16_t hourX_array [13] = {0, 77, 86, 95, 91, 77, 62, 48 , 36, 31, 34, 48, 62};
uint16_t hourY_array [13] = {0, 38, 50, 71, 86, 100, 105, 100, 86, 71, 55, 42, 37};
 int main(void){
  PLL_Init(Bus50MHz);         // bus clock at 50 MHz
	
   SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	 ST7735_InitR(INITR_REDTAB);	//initialize creen
	   ST7735_DrawBitmap(0, 155, clock_face_template, 128, 140 );		//draw static image of analog clock face 15 pixels down from top of screen 

	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  
	 SysTick_Init(80000000);     //initialize SysTick timer for interrupts on every 1s, 80000000:::: for fast mode set to 2000000 and flag from 4 to 1
	 Buttons_Init();
	Timer0A_Init(5000);					//sound and debounce inititialization 
	Timer1_Init();
   EnableInterrupts();

while(1){                 

  if(flag==4 || buttonpress){				//check how many times flag has been incremented, 4 == 1second
				
		if(buttonpress){buttonpress = 0;}  //don't update time just change screens/alarm
		else{		//if flag triggered then increment time
		
		    flag = 0;					//reset SysTick interrupt flag, located in main.h
				PF2 =  PF2 ^ 0x4;	//toggle LED everytime clock gets updated update
			
				//update time
				if(second == 59){			//check to see if second hand rolls over
					second = 0;
					if (minute == 59){	//check to see if minute hand rolls over
						minute = 0;
						if(hour == 12){		//check to see if hour hand rolls over
							hour = 1;
						}
						else if(hour == 11){			//check to see if am/pm rolls over
							am_pm = !am_pm;
							hour = hour + 1;
						}
						else{hour = hour + 1;}		//otherwise just increment +1
					}
					else{minute = minute + 1;}
				}
				else{second = second + 1;}
			}
			//update analog positions	
			old_secX = current_secX;								//save old positions so that they can be erased for the new ones
			old_secY = current_secY;
			old_minX = current_minX;
			old_minY = current_minY;
      old_hourX = current_hourX;
			old_hourY = current_hourY;
			current_secX = secX_array[second];			//update hand positions
			current_secY = secY_array[second]+15;		//Y positions shifted down 15 to fit on screen correctly since bitmap is moved down 15 pixels
			current_minX = secX_array[minute];			//minute hand is same as second hand positions
			current_minY = secY_array[minute]+15;
			current_hourX = secX_array[((hour%12)*5) + (minute/12)];			//move hour hand 1/5 of an hour forward every 12 min
			current_hourY = secY_array[((hour%12)*5) + (minute/12)]+15;		//modulo 12 ensures array doesn't go out of bounds

			if((am_pm == alarm_am_pm)&&(hour == a_hour)&&(minute == a_minute)&&(alarm)){				//if alarm is set and alarm time matches current time
				ring = 1;							//set flag for speaker to ring
			}
			else{ring = 0;}
			
      if(set_alarm){										//if in set alarm mode
				if(reset){							//if coming back from regular mode, reset display
					reset = 0;
					ST7735_SetCursor(0,0);									
					for(int i = 0; i<18;i = i+1){
					ST7735_OutChar(' ');
					}
					ST7735_SetCursor(0,0);	
					DisableInterrupts();					//disable interrupts so screen can be redrawn
					ST7735_DrawBitmap(0, 155, clock_face_template, 128, 140 );		//draw static image of analog clock face 15 pixels down from top of screen 
					EnableInterrupts();
				  }
				old_ahx = ahx;							//save old hand positions so they can be erased later
				old_ahy = ahy;
				old_amx = amx;
				old_amy = amy;
				 ahx =   secX_array[((a_hour%12)*5)];
				 ahy = 	secY_array[((a_hour%12)*5)]+15;	
				 amx = 	secX_array[a_minute];	
				 amy = 	secY_array[a_minute]+15;
				
				DrawLine(64, 86, old_amx, old_amy, ST7735_BLACK);						
				DrawLine(64, 86, old_ahx, old_ahy, ST7735_BLACK);							
			
				DrawLine(64, 86, ahx, ahy, ST7735_GREEN);			//draw hour hand
				DrawLine(64, 86, amx, amy, ST7735_BLUE);				//minute hand
				
				ST7735_SetCursor(0, 0);				//print out digital time at top of screen

				ST7735_OutChar('A');					//output digital alarm setting
				ST7735_OutChar('l');
				ST7735_OutChar('a');
				ST7735_OutChar('r');
				ST7735_OutChar('m');
				ST7735_OutChar(':');
				ST7735_OutChar(' ');
				
				if(a_hour<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(a_hour);
				ST7735_OutChar(':');
				if(a_minute<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(a_minute);

				
				if(alarm_am_pm){
				ST7735_OutChar('p');
				ST7735_OutChar('m');
				}
				else{  
				ST7735_OutChar('a');
				ST7735_OutChar('m');
				}
				
			}
				
		else{																				//if in regular clock mode
			if(reset){	
					reset = 0;//if coming back from alarm set mode
					DisableInterrupts();								//	reset display
					ST7735_SetCursor(0,0);							
					for(int i = 0; i<20;i = i+1){
					ST7735_OutChar(' ');
					}
				ST7735_SetCursor(0,0);	
				ST7735_DrawBitmap(0, 155, clock_face_template, 128, 140 );		//draw static image of analog clock face 15 pixels down from top of screen 
				EnableInterrupts();
				}
			
				DrawLine(64, 86, old_secX, old_secY, ST7735_BLACK);						//erase old hands, no resetting screen image
				DrawLine(64, 86, old_minX, old_minY, ST7735_BLACK);						
				DrawLine(64, 86, old_hourX, old_hourY, ST7735_BLACK);							
			
				DrawLine(64, 86, current_hourX, current_hourY, ST7735_GREEN);			//draw hour hand
				DrawLine(64, 86, current_minX, current_minY, ST7735_BLUE);				//minute hand
				DrawLine(64, 86, current_secX, current_secY, ST7735_RED);					//second hand
				
				ST7735_SetCursor(0, 0);																						//print out digital time at top of screen
				if(hour<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(hour);
				ST7735_OutChar(':');
				if(minute<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(minute);
				ST7735_OutChar(':');
				if(second<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(second);
				ST7735_OutChar(' ');
				
				if(am_pm){
				ST7735_OutChar('p');
				ST7735_OutChar('m');
				}
				else{  
				ST7735_OutChar('a');
				ST7735_OutChar('m');
				}
				//display alarm status
				ST7735_SetCursor(0, 2);								//display alarm staus whether armed or not
				if(alarm){ST7735_OutChar('X');}			//display 'X' when alarm is on
				else{ST7735_OutChar(' ');}
				ST7735_SetCursor(0, 0);
				
			}
		}
  }
}



void DrawLine(int x1, int y1, int x2, int y2, int color){

	int i= 0;
	int x= 0;					//absolute xy distance
	int y = 0;					
	int pixel_x = 0;	//new pixel location coordinates
	int	pixel_y = 0;
	int dx,dy,sdx,sdy,dxabs,dyabs = 0; //variables for finding sign, abs valeu, range between x2-x1, y2-y1

  dx=x2-x1;      // the horizontal distance of the line
  dy=y2-y1;      // the vertical distance of the line 
  if(dx <0){ dxabs = (~dx + 1);}    //get abs value
	else {dxabs = dx;}
  if(dy <0){ dyabs = (~dy + 1);}
	else {dyabs = dy;}

  if(dx<0){sdx = -1;}			//determine sign of the range/direction from center
	else{sdx = 1;}
  if(dy<0){sdy = -1;}
	else{sdy = 1;}
  x=dyabs>>1;			//divide actual values by 2
  y=dxabs>>1;				
  pixel_x=x1;							//pixels to draw
  pixel_y=y1;

  if (dxabs>=dyabs) // the line is more horizontal than vertical
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        pixel_y+=sdy;
      }
      pixel_x+=sdx;
      ST7735_DrawPixel(pixel_x,pixel_y,color);			//draw pixels
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        pixel_x+=sdx;
      }
      pixel_y+=sdy;
      ST7735_DrawPixel(pixel_x,pixel_y,color);
    }
  }
}
