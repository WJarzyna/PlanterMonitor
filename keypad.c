#include "keypad.h"

volatile uint8_t kbstate;

ISR(PCINT2_vect)
{
    kbstate = (~PINC) & 0xF0;
    if( kbstate ) beep();
}

void beep()
{
    BUZ_ON;
    _delay_ms(50);
    BUZ_OFF;
}

uint8_t wait_key()
{
	uint8_t input;

    while( !kbstate );
    input = kbstate;
    while( kbstate );
    
    switch( input )
    {
        case K_UP:;
        case K_DN:;
        case K_OK:;
        case K_ESC: return input;
        default: return 0;
    }
}

void init_kp_buz()
{
  PORTC = 0xF0;//pullup
  PCICR = (1<<PCIE2);//kb int enable
  PCMSK2 = 0xF0;//unmask

  TCCR2A = (1<<COM2A0);//OCR2A enable - buzzer
  TCCR2B = (1<<CS20);//clock 1MHz
  DDRD = (1<<7);//buzzer output enable
}  
