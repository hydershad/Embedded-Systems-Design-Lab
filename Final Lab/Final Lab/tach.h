//HYDER SHAD
//UT EID: hs25796
//EE 445L Lab 10
//TTH 12:30 - 2pm
//11/30/2018
//tach.h

#include <stdint.h>

void Tach_Init(void);

uint32_t Tach_GetPeriod(void);

uint32_t Tach_GetPeriod(void);

uint32_t mspeed(void) ;
uint32_t targetspeed(void) ;
void set_ki1(uint32_t new_Ki1);

void set_ki2(uint32_t new_Ki2);
void set_kp1(uint32_t new_Kp1);
void set_kp2(uint32_t new_Kp2);

void set_PIdelay(uint32_t new_delay);

void set_targetspeed(uint32_t new_speed);

