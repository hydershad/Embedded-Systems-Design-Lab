// Switch.h


#include <stdbool.h>

/**
 * Initialize the switches on the correct port, the pins, and the
 * interrupts necessary to handle button presses.
 */
void Buttons_Init(void);


void Buttons_10ms_Handler(void);
