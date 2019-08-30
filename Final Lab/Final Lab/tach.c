//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 10
//TTH 12:30 - 2pm
//11/30/2018

//tach.c
//tachometer input trigger from op-amp output, PI control module
#include "tm4c123gh6pm.h"

#include <stdint.h>
#include <stdio.h>
#include "PWM.h"
#include "ST7735.h"

#define PC5 										(*((volatile uint32_t *)0x40006080))	// PC5 Direct address
#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable

#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACMR        0x00000004  // GPTM TimerA Capture Mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_IMR_CAEIM         0x00000004  // GPTM CaptureA Event Interrupt
                                            // Mask
#define TIMER_ICR_CAECINT       0x00000004  // GPTM CaptureA Event Interrupt
                                            // Clear
#define TIMER_TAILR_M           0xFFFFFFFF  // GPTM Timer A Interval Load
            
uint32_t M_Period = 0;
int32_t Overflow = 0;
uint32_t static First;        // Timer0A first edge, 12.5 ns units
uint16_t flag = 0;
uint32_t M_Target_Speed= 20;
uint32_t M_Speed = 0;      		 // motor speed in 0.1 rps
//******MOVED TO MAIN PROGRAM LOOP****/

/*

int32_t M_Error = 0;           // speed error in 0.1 rps
int32_t M_Duty= 30000;         // duty cycle 40 to 39960
uint32_t M_PI_delay = 5; // delay counter for PID control
uint32_t M_PI_delay_value = 5;

uint32_t I, P = 0;
uint32_t Ki1, Ki2 = 20;  //Integral variables
uint32_t Kp1, Kp2 = 20; //Proportional variables
*/
long StartCritical (void);
void EndCritical(long sr); 
void EnableInterrupts(void);  // Enable interrupts


int Done;
void Tach_Init(void){

	SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  SYSCTL_RCGCGPIO_R |= 0x02;       // activate port B

  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 input
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6
  GPIO_PORTB_DEN_R |= 0x40;        // configure PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  TIMER0_CTL_R &= ~0x00000001;     // disable timer0A during setup
  TIMER0_CFG_R = 0x00000004;       // configure for 16-bit capture mode
  TIMER0_TAMR_R = 0x00000007;      // configure for rising edge event
  TIMER0_CTL_R &= ~0x0000000C;     // rising edge
  TIMER0_TAILR_R = 0x0000FFFF;     // start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit 
  TIMER0_IMR_R |= 0x00000004;      // enable capture match interrupt
  TIMER0_ICR_R = 0x00000004;       // clear timer0A capture match flag
  TIMER0_CTL_R |= 0x00000001;      // timer0A 24-b, +edge, interrupts
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; //Timer0A=priority 2
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC

}

void Timer0A_Handler(void){
	TIMER0_ICR_R = 0x00000004;       // acknowledge timer0A

	M_Period = (First - TIMER0_TAR_R)&0x00FFFFFF;		//find period for tachometer pulses to calculate speed
	if(M_Period > 10000000) { M_Period = 10000000; }
// 24-bit, 12.5ns resolution
	
	M_Speed = 200000000/M_Period; 	// 1 rps
	
};

//******MOVED TO MAIN PROGRAM LOOP****/
//These functions are for setting the PI variables and sending motor speed to Blynk

uint32_t Tach_GetPeriod(void) {
	return M_Period;
}

uint32_t mspeed(void) {
	ST7735_SetCursor(8, 0);
	ST7735_OutString("      ");
	ST7735_SetCursor(8, 0);
	ST7735_OutUDec(M_Speed);
	return M_Speed;
}
uint32_t targetspeed(void) {
	ST7735_SetCursor(8, 1);
	ST7735_OutString("      ");
	ST7735_SetCursor(8, 1);
	ST7735_OutUDec(M_Target_Speed);
	return M_Target_Speed;
	
}
/*
void set_ki1(uint32_t new_Ki1){
	Ki1 = new_Ki1;
	ST7735_SetCursor(8, 0);
	ST7735_OutString("      ");
	ST7735_SetCursor(8, 0);
	ST7735_OutUDec(new_Ki1);

	}

void set_ki2(uint32_t new_Ki2){
	Ki2 = new_Ki2;
	
	ST7735_SetCursor(8, 1);
	ST7735_OutString("      ");
	ST7735_SetCursor(8, 1);
	ST7735_OutUDec(new_Ki2);

	}
void set_kp1(uint32_t new_Kp1){
	Kp1 = new_Kp1;
}
void set_kp2(uint32_t new_Kp2){
	Kp2 = new_Kp2;
}
void set_PIdelay(uint32_t new_delay){
	M_PI_delay_value	= new_delay;
}

void set_targetspeed(uint32_t new_speed){
	M_Target_Speed = new_speed;
}


*/
