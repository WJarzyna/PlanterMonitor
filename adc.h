#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

uint8_t read_adc_8b(uint8_t pin); //pin in decimal

void adc_init();


#endif /* ADC_H_ */
