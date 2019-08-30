//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 4
//TTH 12:30 - 2pm
//10/5/2018
//Blynk.c
// -------------------------------------------------------------------
// Description: This code is used to bridge the TM4C123 board and the Blynk Application
//              via the ESP8266 WiFi board
// Author: Mark McDermott and Andrew Lynch (Arduino source)
// Converted to EE445L style Jonathan Valvano
// Orig gen date: May 21, 2018
// Last update: Sept 20, 2018
//
// Download latest Blynk library here:
//   https://github.com/blynkkk/blynk-library/releases/latest
//
//  Blynk is a platform with iOS and Android apps to control
//  Arduino, Raspberry Pi and the likes over the Internet.
//  You can easily build graphic interfaces for all your
//  projects by simply dragging and dropping widgets.
//
//   Downloads, docs, tutorials: http://www.blynk.cc
//   Sketch generator:           http://examples.blynk.cc
//   Blynk community:            http://community.blynk.cc
//
//------------------------------------------------------------------------------

// TM4C123       ESP8266-ESP01 (2 by 4 header)
// PE5 (U5TX) to Pin 1 (Rx)
// PE4 (U5RX) to Pin 5 (TX)
// PE3 output debugging
// PE2 nc
// PE1 output    Pin 7 Reset
// PE0 input     Pin 3 Rdy IO2
//               Pin 2 IO0, 10k pullup to 3.3V  
//               Pin 8 Vcc, 3.3V (separate supply from LaunchPad 
// Gnd           Pin 4 Gnd  
// Place a 4.7uF tantalum and 0.1 ceramic next to ESP8266 3.3V power pin
// Use LM2937-3.3 and two 4.7 uF capacitors to convert USB +5V to 3.3V for the ESP8266
// http://www.ti.com/lit/ds/symlink/lm2937-3.3.pdf
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "Timer2.h"
#include "Timer3.h"
#include "UART.h"
#include "PortF.h"
#include "esp8266.h"
//////
#include "SysTickInts.h"
#include "Switch.h"
#include "Timer1.h"
#include "HandPositions.h"
#include "PeriodicSysTickInts.h"

void EnableInterrupts(void);    // Defined in startup.s
void DisableInterrupts(void);   // Defined in startup.s
void WaitForInterrupt(void);    // Defined in startup.s
void DrawLine(int startX, int startY, int endX, int endY, int color);

uint32_t LED;      // VP1
uint32_t LastF;    // VP74
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;
 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){
// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
/*#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
  */         
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x01)  {  //for debug purposes
      LED = pin_int;
      PortF_Output(LED<<2); // Blue LED
		}
																
		    if(pin_num == 0x02)  { 							//match virtual pin input connections to a specific button
					if(pin_int)Buttons_Pressed(3);		//if pin_int is not zero, button is pressed, call button pressed function to control clock settings
		}
				if(pin_num == 0x03)  {  
					if(pin_int)Buttons_Pressed(2);
		}
			  if(pin_num == 0x04)  {  
					if(pin_int)Buttons_Pressed(0);
		}
				if(pin_num == 0x05)  {  
					if(pin_int)Buttons_Pressed(1);
		}
/*#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
  */ 
			// Parse incoming data        
/*#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
 */ }  
}

uint32_t oldah;		//variables to keep track of  alarm settings to prevent excess data transfer
uint32_t oldam;
uint32_t oa;
void SendInformation(void){
  
// your account will be temporarily halted if you send too much data
    TM4C_to_Blynk(74, hour);  	// VP74 (hour display)
		TM4C_to_Blynk(75, minute);  // VP75 (minute display)
		TM4C_to_Blynk(76, second);  // VP76	(second display)

	if(oldah != a_hour){					// push new data only if alarm settings have been modified
		TM4C_to_Blynk(77, a_hour);  // VP77 (alarm hour display)
	}
	if(oldam != a_minute){			  // push new data only if alarm settings have been modified
				TM4C_to_Blynk(78, a_minute);  // VP78 (alarm hour display)

	}
	if(oa != alarm){								// push new data only if alarm settings have been modified
		TM4C_to_Blynk(79, alarm); 		// VP79 toggle alarm on and off
	}
oldah = a_hour;										//keep track of old values for alarm time and if alarm is armed to avoid sending too much data
oldam = a_minute;
oa =  alarm;
	
	/*#ifdef DEBUG3								//need debug3 but don't want this debug section active
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
		#endif
		*/
  }
 
  
int main(void){       
  PLL_Init(Bus80MHz);   // Bus clock at 80 MHz
  DisableInterrupts();  // Disable interrupts until finished with inits
  PortF_Init();					//enable LEDs for clock and debug
  Output_Init();        // initialize ST7735
	
	
	#ifdef DEBUG3				//print out lab title
  ST7735_OutString("EE445L Lab 4D\nBlynk example\n");
#endif
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\rBlynk example");
#endif

  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C, 800000);		// check for receive data from Blynk App every 10ms
  Timer3_Init(&SendInformation, 40000000); // Send data back to Blynk App every 1/2 second
	
	Output_Init();        // reset ST7735 again
	ST7735_DrawBitmap(0, 155, clock_face_template, 128, 140 );		//draw static image of analog clock face 15 pixels down from top of screen 
	 SysTick_Init(80000000);   //initialize SysTick timer for interrupts on every 1s, 80000000:::: for fast mode set to 2000000 and flag from 4 to 1
	Timer0A_Init(5000);					//sound and debounce inititialization 
  EnableInterrupts();

while(1) {		//run Lab3 clock and UI code
		
	if(flag==4 || buttonpress){				//check how many times flag has been incremented, 4 == 1second
				
		if(buttonpress){buttonpress = 0;}  //don't update time just change screens/alarm
		else{									//if flag triggered then increment time
		
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
			else{ring = 0;}				//turn off alarm sound after minute has passed/incremented
			
      if(set_alarm){										//if in set alarm mode
				if(reset){										//if coming back from regular mode, reset display
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
			  amx = 	secX_array[a_minute];				//update hand positions
				amy = 	secY_array[a_minute]+15;		//Y positions shifted down 15 to fit on screen correctly since bitmap is moved down 15 pixels
			  ahx =   secX_array[((a_hour%12)*5)];			 //move hour hand 1/5 of an hour forward every 12 min
				ahy = 	secY_array[((a_hour%12)*5)]+15;	   //modulo 12 ensures array doesn't go out of bounds
				
				DrawLine(64, 86, old_amx, old_amy, ST7735_BLACK);		//erase old hand positions				
				DrawLine(64, 86, old_ahx, old_ahy, ST7735_BLACK);							
			
				DrawLine(64, 86, ahx, ahy, ST7735_GREEN);				//draw hour hand
				DrawLine(64, 86, amx, amy, ST7735_BLUE);				//minute hand
				
				ST7735_SetCursor(0, 0);			//output digital alarm settings for hour and minute and pm/am

				ST7735_OutChar('A');				//					
				ST7735_OutChar('l');
				ST7735_OutChar('a');				//
				ST7735_OutChar('r');
				ST7735_OutChar('m');				//	
				ST7735_OutChar(':');
				ST7735_OutChar(' ');
				
				if(a_hour<10){ST7735_OutUDec(0);}		//
				ST7735_OutUDec(a_hour);
				ST7735_OutChar(':');								//
				if(a_minute<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(a_minute);						//
				
				if(alarm_am_pm){										//	
				ST7735_OutChar('p');
				ST7735_OutChar('m');								//	
				}
				else{  
				ST7735_OutChar('a');								//
				ST7735_OutChar('m');
				}
				
			}
				
		else{									//if in regular clock mode
			if(reset){	
					reset = 0;			//if coming back from alarm set mode
					DisableInterrupts();								//	reset display
					ST7735_SetCursor(0,0);							
					for(int i = 0; i<20;i = i+1){				//clear old text
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
				
				ST7735_SetCursor(0, 0);								//print out digital time at top of screen
				if(hour<10){ST7735_OutUDec(0);}
				ST7735_OutUDec(hour);									//		
				ST7735_OutChar(':');
				if(minute<10){ST7735_OutUDec(0);}			//
				ST7735_OutUDec(minute);
				ST7735_OutChar(':');									//
				if(second<10){ST7735_OutUDec(0);}			
				ST7735_OutUDec(second);								//
				ST7735_OutChar(' ');
				
				if(am_pm){														//	
				ST7735_OutChar('p');	
				ST7735_OutChar('m');									//
				}
				else{  																//
				ST7735_OutChar('a');									
				ST7735_OutChar('m');									//
				}
							
				ST7735_SetCursor(0, 2);							//display alarm staus about whether its armed or not
				if(alarm){ST7735_OutChar('X');}			//display 'X' when alarm is on
				else{ST7735_OutChar(' ');}
				ST7735_SetCursor(0, 0);
				
			}
		}
  }
}


// Lab3 DrawLine function code
void DrawLine(int x1, int y1, int x2, int y2, int color){		//draws lines between (x1, y1) and (x2, y2) coordinates to construct analog clock hands

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
  x=dyabs>>1;							//divide actual values by 2
  y=dxabs>>1;				
  pixel_x=x1;							//pixels to draw
  pixel_y=y1;

  if (dxabs>=dyabs) 			// the line is more horizontal than vertical
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;							//adjust y pixel draw rate
      if (y>=dxabs)
      {
        y-=dxabs;
        pixel_y+=sdy;
      }
      pixel_x+=sdx;
      ST7735_DrawPixel(pixel_x,pixel_y,color);			//draw pixels
    }
  }
  else 		//the line is more vertical than horizontal 
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;								//adjust x pixel draw rate
      if (x>=dyabs)
      {
        x-=dyabs;
        pixel_x+=sdx;
      }
      pixel_y+=sdy;						
      ST7735_DrawPixel(pixel_x,pixel_y,color);			//draw pixels
    }
  }
}


