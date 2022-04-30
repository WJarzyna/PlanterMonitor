#include "planter_lcd.h"
#include "adc.h"

#define JTAG_OFF MCUCR|=(1<<JTD);MCUCR|=(1<<JTD)
/*
zalozenie: fi toczna 44cm
*/

#define LEVEL_SAFETY_RESET 15
#define LEVEL_SAFETY_LOW 3


#define NOSPEEDCHECK 0
#define SPEEDCHECK 0xFF

#define SLIP_RATIO 2

volatile uint8_t packer_raw=0, kbstate=0;
volatile uint16_t planter_raw=0;
volatile uint32_t  packer_cnt=0, planter_cnt=0;


void init()
{
    JTAG_OFF;

    TCCR0B=(1<<CS02)|(1<<CS00);    //T0 clk /1024
    TIMSK0=1;   //T0 ovf int enable

    init_kp_buz();

    beep();

    TCCR1B=(1<<ICNC1)|(1<<CS11);//noise cancel on, clk/8
    TIMSK1=(1<<5)|(1<<0);//ovf and ic int enable

    EICRA=(1<<ISC01);   //INT0 falling edge
    EIMSK=(1<<INT0);    //INT0 enable

    adc_init();

    lcd_start();

    sei();
}

uint8_t check_stop()
{
    if(planter_raw==0xFFFF && packer_raw!=0xFF) return ERR_PLANTER_STOP;
    if(packer_raw==0xFF && planter_raw!=0xFFFF) return ERR_PACKER_STOP;
    return 0;
}

uint8_t check_warn(uint8_t level_l, uint8_t level_r, uint8_t speedcheck)
{
    static uint8_t level_fail;

    if(speedcheck)
    {
        uint16_t planter_v = PLANTER_V/planter_raw;
        uint16_t packer_v = PACKER_V/packer_raw;

        if(packer_v > SLIP_RATIO*planter_v) return ERR_PLANTER_SLIP;
        if(planter_v > SLIP_RATIO*packer_v) return ERR_PACKER_SLIP;
    }

    if(level_r<LEVEL_SAFETY_LOW && ( !(level_fail&R_FAIL) ))
    {
        level_fail |= R_FAIL;
        return ERR_LOW_LEVEL_R;
    }

    if(level_l<LEVEL_SAFETY_LOW && ( !(level_fail&L_FAIL) ))
    {
        level_fail |= L_FAIL;
        return ERR_LOW_LEVEL_L;
    }

    if(level_r>=LEVEL_SAFETY_RESET) level_fail &= (~R_FAIL);
    if(level_l>=LEVEL_SAFETY_RESET) level_fail &= (~L_FAIL);

    return 0;
}

void throw_error(uint8_t errno)
{
    lcd_clear();

    BUZ_ON;
    lcd_mvputs( 0, 0, T_ERR);
    lcd_mvputs( 0, 1, errno&0x0F ? T_PACKER_STOP : T_PLANT_STOP);

    while(1)
    {
        errno=check_stop() & 0xF0;
        if(!errno) break;
    }
    BUZ_OFF;
}

void throw_warn(uint8_t warnno, uint8_t speedcheck)
{
    uint8_t speed_err=0;

    lcd_clear();
    beep();
    lcd_mvputs( 0, 0, T_WARN);

    switch(warnno&0xF0)
    {
        case ERR_LOW_LEVEL:
        {
            lcd_mvputs( 0, 1, T_LOW);
            lcd_mvputs( 0, 2, warnno&0x02 ? T_R : T_L);
            break;
        }

        case ERR_SLIP: lcd_mvputs( 0, 1, warnno&0x0F ? T_PLANT_SLIP : T_PACKER_SLIP); break;
    }

    _delay_ms(200);
    beep();

    for(uint8_t t=200;t>0;--t)
    {
        speed_err=check_stop() & speedcheck;
        if(speed_err)
        {
            throw_error(speed_err);
            break;
        }
        _delay_ms(20);
    }
}


int main(void)
{
	const char* service_menu[SERVICE_MENU_ENTRIES_NO] = SERVICE_MENU_ENTRIES;
	const char* main_menu[MAIN_MENU_ENTRIES_NO] = MAIN_MENU_ENTRIES;

    init();

    welcome_screen();

    while(1)
    {
        switch( menu( main_menu, MAIN_MENU_ENTRIES_NO) )
        {
            case MODE_COMBI: run_planter(SPEEDCHECK);  break;

            case MODE_HARROW: run_harrow(); break;

            case MODE_PLANTER: run_planter(NOSPEEDCHECK); break;

            case MODE_PLANTER_CAL: planter_cal(); break;

            case MODE_SERVICE:
            	{
            		switch( menu( service_menu, SERVICE_MENU_ENTRIES_NO) )
					{
						case 1: speed_diag(); break;
						case 2: analog_diag(); break;
						case 3: analog_cal(); break;
						case 4: warn_settings(); break;
					}
					_delay_ms(1000);
					lcd_clear();
					break;
            	}
        }
    }
}

ISR(TIMER0_OVF_vect)
{
    packer_raw=0xFF;
}

ISR(INT0_vect)
{
    packer_raw=TCNT0;
    TCNT0=0;
    ++packer_cnt;
}

ISR(TIMER1_OVF_vect)
{
    planter_raw=0xFFFF;
}

ISR(TIMER1_CAPT_vect)
{
    planter_raw=ICR1;
    TCNT1=0;
    ++planter_cnt;
}
