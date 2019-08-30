

#include <stdint.h>

#ifndef DAC_H
#define DAC_H 1

/**
 * Initialize the DAC, setting up any necessary ports and pins.
 */
void DAC_Init(uint16_t initialData);

/**
 * Set the DAC output voltage to the specified value.
 * The output is limited to 12 bits so `outputVoltage` will be truncated to 12 bits before setting the output.
 */
void DAC_Output(uint16_t outputVoltage);

#endif

