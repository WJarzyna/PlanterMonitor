#include "planter.h"

#define JTAG_OFF MCUCR|=(1<<JTD);MCUCR|=(1<<JTD)

#define NOSPEEDCHECK 0
#define SPEEDCHECK 1


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
