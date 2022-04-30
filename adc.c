#include "adc.h"

uint8_t read_adc_8b(uint8_t pin) //pin in decimal
{
    ADMUX&=0xF0;
    ADMUX|=pin;
    ADCSRA|=(1<<ADSC);
    while(!(ADCSRA&(1<<ADIF)));
    ADCSRA|=(1<<ADIF);
    return ADCH;
}

void adc_init()
{
	ADMUX=(1<<ADLAR)|(1<<REFS0)|(1<<REFS1);//ADC left adjust 2.56 ref
	ADCSRA=(1<<ADEN);//enabble ADC
    DIDR0=(1<<0)|(1<<1);//analog pin config
}
