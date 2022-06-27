#include "keypad.h"

volatile uint8_t kbstate;
volatile uint16_t systick;

ISR(PCINT2_vect)
{
    kbstate = (~PINC) & 0xF0;
    if( kbstate ) beep();
}

ISR( TIMER2_OVF_vect )
{
	++systick;
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

uint8_t check_delay( uint8_t time_sec )
{
	static uint8_t delay;

	if( time_sec )
	{
		systick = 0;
		delay = time_sec;
		return 0;
	}

	if( delay > systick>>12 ) return 0;

	return 1;
}

void init_kp_buz()
{
  PORTC = 0xF0;//pullup
  PCICR = (1<<PCIE2);//kb int enable
  PCMSK2 = 0xF0;//unmask

  TCCR2A = (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);//OCR2A fast PWM - buzzer, 4kHz
  TCCR2B = (1<<CS20);//clock 1MHz
  TIMSK2 = (1<<TOIE); //ovf int enable
  DDRD = (1<<7);//buzzer output enable
}  
