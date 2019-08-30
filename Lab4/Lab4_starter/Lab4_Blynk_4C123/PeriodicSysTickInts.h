//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 4
//TTH 12:30 - 2pm
//10/5/2018
//PeriodicSysTickInts.h

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

//global variables for use across functions
int reset = 0;							//used to track if user changed between current time screen and set alarm screen
uint16_t hour = 2;					//keep track of time
uint16_t minute = 50;
uint16_t second = 0;
uint16_t a_hour = 2;					//keep track of time
uint16_t a_minute = 52;
uint16_t am_pm = 0;	
uint16_t alarm_am_pm = 0;

uint16_t ring = 0;
uint16_t alarm = 0;					//alarm on/off
uint16_t set_alarm = 0;			//set up alarm button status
uint16_t flag = 0; 					//flag for interrupt handler to set when 1s elasped
uint16_t ahx = 0;						//alarm display hand positions
uint16_t ahy = 0;
uint16_t amx = 0;
uint16_t amy = 0;
uint16_t old_ahx = 0;				//old hand positions
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
uint16_t old_hourX = 0;				//old hand positions
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

