#include "planter.h"

#define JTAG_OFF MCUCR|=(1<<JTD);MCUCR|=(1<<JTD)

#define NOSPEEDCHECK 0
#define SPEEDCHECK 1


void init()
{
    JTAG_OFF;

    init_kp_buz();

    beep();

    lcd_start();

    check_cal();

    init_sensors();
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
