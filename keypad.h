#pragma once

#include <avr/interrupt.h>
#include <util/delay.h>

#define BUZ_ON TCCR2B=(1<<CS20)
#define BUZ_OFF TCCR2B=0; PORTD&=~(1<<7)

#define K_UP 0x10
#define K_DN 0x80
#define K_OK 0x40
#define K_ESC 0x20
#define K_1 0x30
#define K_2 0x60
#define K_3 0xC0
#define K_4 0x50
#define K_5 0xA0
#define K_6 0x90
#define K_7 0x70
#define K_8 0xE0
#define K_9 0xB0
#define K_0 0xD0

void beep();
uint8_t wait_key();
void init_kp_buz();
