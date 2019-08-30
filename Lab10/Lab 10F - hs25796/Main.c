//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 10
//TTH 12:30 - 2pm
//11/30/2018
//main.c


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

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "PLL.h"
#include "PWM.h"
#include "tach.h"
#include "ST7735.h"
#include "SysTickInts.h"
#include "UART.h"
#include "esp8266.h"
#include "Timer2.h"
#include "Timer3.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
//void WaitForInterrupt(void);  // low power mode
void TM4C_to_Blynk(uint32_t pin,uint32_t value);
void Blynk_to_TM4C(void);
void SendInformation(void);
int32_t x = 0;								//used for LCD plot width position
 uint32_t Target_Speed = 30;				// initial 30 rps
 uint32_t Speed = 0;      				// motor speed in 0.1 rps
 int32_t Error = 0;           		// speed error in 0.1 rps
 int32_t Duty = 30000;           			// duty cycle 40 to 39960
 uint32_t PI_delay = 10; 					// delay counter for PID control loop
uint32_t PI_delayvalue = 10;			//delay reload
 
uint32_t I, P = 0;				//integral and proportional terms
uint32_t Ki1, Ki2 = 20;   //Integral constants 
uint32_t Kp1, Kp2 = 20; 	//Proportional constants

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
  
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x06)  {  	//update motor target speed
			Target_Speed = pin_int;	
		}														
		else if(pin_num == 0x01)  { 	//update kp1
			Kp1 = pin_int;
		}
		else if(pin_num == 0x02)  {  //update kp2
			Kp2 = pin_int;

		}
		else if(pin_num == 0x03)  {  //update ki1
			Ki1 = pin_int;

		}
		else if(pin_num == 0x04)  {  //update ki2
			Ki2 = pin_int;

		}
		else if(pin_num == 0x05)  {  //update PI loop delay
			PI_delay = pin_int;
		}
	}
}

void SendInformation(void){
  
// your account will be temporarily halted if you send too much data
    TM4C_to_Blynk(79, mspeed());  //rps value
		TM4C_to_Blynk(80, mspeed());  //rps gauge
	
  }


int main(void) {

	PLL_Init(Bus80MHz);         // bus clock at 80 MHz
	DisableInterrupts();  			// Disable interrupts until finished with inits
	ST7735_InitR(INITR_REDTAB);	//initialize LCD
	ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  Tach_Init();										// initialize input capture
	ST7735_InitR(INITR_REDTAB);			//initialize LCD again to clear wifi setup debug output
	PWM0B_Init(40000, 30000); 			//Set initial duty cycle and overall period
	
	//Timer2_Init(&Blynk_to_TM4C, 800000);			// check for receive data from Blynk App every 10ms
	//Timer3_Init(&SendInformation, 40000000); 	// Send data back to Blynk App every 1/2 second
	SysTick_Init(0x0FFFFFF);									//use send and recieve data to/from blynk server
	
	
	EnableInterrupts();

	
	ST7735_SetCursor(0,0);
	ST7735_OutString("speed:\target:");
	while(1) {
			if(PI_delay == 0){			//PI controller
		Speed = mspeed();
		Target_Speed = targetspeed();
		Error = Target_Speed-Speed;   // 0.1 rps
		P = (Kp1*Error)/Kp2;
		I = I +((Ki1*Error)/Ki2);  // discrete integral
		Duty = (P + I);
		if(Duty < 15000) Duty=15000;    //limit output min and max
		if(Duty>39900) Duty=39900;      
		PWM0B_Duty(Duty);            //update PWM duty cycle output
				
		int32_t speedY = ST7735_TFTHEIGHT - 20 - Speed / 4;		//Draw RPS Plot on LCD
		//ST7735_OutUDec(Speed);
		//ST7735_SetCursor(8, 1);
		//ST7735_OutUDec(Target_Speed);
		if(x == 0) ST7735_PlotClear(32, 159);
		
		ST7735_DrawPixel(x+1, speedY+1, ST7735_RED);	//
		ST7735_DrawPixel(x+1, speedY, ST7735_RED);
		ST7735_DrawPixel(x, speedY+1, ST7735_RED);
		ST7735_DrawPixel(x, speedY, ST7735_RED);
		x = (x + 1) % ST7735_TFTWIDTH;
		PI_delay = PI_delayvalue;
	}
	else {
		PI_delay = PI_delay - 1;		//PI_delay value for how fast the controller loop is run
	}
	
		
	}
}

void SysTick_Handler(void){
		Blynk_to_TM4C();		//check for new information
		SendInformation();	//send target speed and actual speed to Blynk
}




