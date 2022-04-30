#include "planter_lcd.h"

extern volatile uint8_t packer_raw, kbstate;
extern volatile uint16_t planter_raw;
extern volatile uint32_t  packer_cnt, planter_cnt;

void print_data(uint16_t area, uint16_t speed)
{
    lcd_set_cursor(9,1);
    lcd_put_f(speed);

    lcd_set_cursor(9,2);
    lcd_put_f(area);
}

void set_back_text()
{
    lcd_clear();

    lcd_mvputs( 0, 1, T_VEL);
    lcd_mvputs( 0, 2, T_AREA);
}

void welcome_screen()
{
    lcd_clear();
    lcd_mvputs( 0, 0, T_VERSION);
    _delay_ms(1000);
    beep();
}

void lcd_start()
{
	lcd_init();
    lcd_on();

    uint8_t arrow_up[8] = MAP_ARR_UP;
    uint8_t arrow_down[8] = MAP_ARR_DOWN;
    lcd_create_char(0, arrow_up);
    lcd_create_char(1, arrow_down);
}

uint8_t menu( const char* entries[], const uint8_t entries_no )
{
    static uint8_t usr_select=1, visible_el=1;

    lcd_clear();

    lcd_mvputs( 0, 3, T_USER_CHMOD);
    lcd_set_cursor(0,3);

    lcd_write(0);
    lcd_mvputs( 1, 3, T_KEYDEF);
    lcd_write(1);

    while(1)
    {
        lcd_mvputs( 0, 1,  entries[visible_el-1]);
        lcd_mvputs( 0, 2,  entries[visible_el]);
        lcd_set_cursor(0, (usr_select!=1 ? 2 : 1) );
        lcd_write(0x7E);

        switch(wait_key())
        {
            case K_DN: usr_select++; break;
            case K_UP: usr_select--; break;
            case K_OK: return usr_select;
            default: break;
        }

        if(usr_select > entries_no ) usr_select = entries_no;
        if(usr_select < 1 ) usr_select=1;

        visible_el= (usr_select==1 ? 1 : usr_select-1);
    }

    return 0;
}

void run_planter(uint8_t speedcheck)
{
    uint8_t exit=0, level_r=0, level_l=0, speed_err=0, warnno=0;

    uint8_t raw_l, raw_r;

    uint8_t calval_ll= eeprom_read_byte(CAL_VAL_LL_ADDR);
    uint8_t calval_rl= eeprom_read_byte(CAL_VAL_RL_ADDR);
    uint8_t calval_l_scale= eeprom_read_byte(CAL_VAL_LH_ADDR) - calval_ll;
    uint8_t calval_r_scale = eeprom_read_byte(CAL_VAL_RH_ADDR) - calval_rl;

    uint8_t speed_warn_flag = eeprom_read_byte(WARN_SETTING_ADDR);

    while(!exit)
    {
        lcd_clear();
        set_back_text();

        while(!exit)
        {
            speed_err=check_stop() & speedcheck;
            if(speed_err)
            {
                throw_error(speed_err);
                break;
            }

            raw_l = read_adc_8b(0);
            raw_r = read_adc_8b(1);

            level_l = raw_l < calval_ll ? 0 : ((uint16_t)(raw_l-calval_ll)*20) / calval_l_scale;
            level_r = raw_r < calval_rl ? 0 : ((uint16_t)(raw_r-calval_rl)*20) / calval_r_scale;

            warnno = check_warn(level_l,level_r,speedcheck&&speed_warn_flag);

            if(warnno)
            {
                throw_warn( warnno, speedcheck);
                break;
            }

            print_data( planter_cnt/PLANTER_A, ( planter_raw==0xFFFF ? 0 : (PLANTER_V/planter_raw) ) );

            lcd_bar(level_l, 0);
            lcd_bar(level_r, 3);

            if(kbstate==K_ESC) exit=1;
        }
    }
}

void run_harrow()
{
    lcd_clear();
    set_back_text();

    while(1)
    {
        print_data(packer_cnt/PACKER_A, (packer_raw==0xFF ? 0 : (PACKER_V/packer_raw) ) );

        if(kbstate==K_ESC) break;
    }
}

void planter_cal()
{
	uint8_t beep_pos[3] = PLANTER_CAL_NO;
	uint8_t beep_cnt = 0;

    lcd_clear();
    lcd_mvputs( 0, 0, T_PCAL);

    for(planter_cnt = 0; planter_cnt < PLANTER_A && kbstate != K_ESC;)
    {
    	lcd_bar((planter_cnt*20)/PLANTER_A, 1);
    	if( planter_cnt == beep_pos[beep_cnt] )
    	{
    		beep();
    		beep_cnt++;
    	}
    }

    if(beep_cnt > 2)
    {
    	BUZ_ON;
    	lcd_clear();
    	lcd_mvputs( 0, 1, T_PCAL_FIN);
    	_delay_ms(1500);
    	BUZ_OFF;
    }
}

void speed_diag()
{
    lcd_clear();
    lcd_mvputs( 0, 0, T_DIAG_VAL);
    lcd_mvputs( 0, 3, T_DIAG_KB);

    while(1)
    {
        lcd_mvputs( 0, 1, T_SDIAG_1);
        lcd_put_uint8(packer_raw);
        lcd_set_cursor(12,1);
        lcd_put_uint32(packer_cnt);

        lcd_mvputs( 0, 2, T_SDIAG_2);
        lcd_put_uint8(planter_raw/256);
        lcd_set_cursor(12,2);
        lcd_put_uint32(planter_cnt);

        lcd_set_cursor(12,3);
        lcd_put_uint8(kbstate);

        if (kbstate==K_ESC) break;
    }
}


void analog_diag()
{
    lcd_clear();
    lcd_mvputs( 0, 0, T_ANALOG_DIAG);
    uint8_t calval_ll = eeprom_read_byte(CAL_VAL_LL_ADDR), calval_rl = eeprom_read_byte(CAL_VAL_RL_ADDR);
    uint8_t calval_lh = eeprom_read_byte(CAL_VAL_LH_ADDR), calval_rh = eeprom_read_byte(CAL_VAL_RH_ADDR);

    while(1)
    {
        lcd_mvputs( 0, 1, T_ADIAG_L);
        lcd_put_uint8(read_adc_8b(0));
        lcd_set_cursor(13,1);
        lcd_put_uint8(calval_ll);
        lcd_set_cursor(17,1);
        lcd_put_uint8(calval_lh);

        lcd_mvputs( 0, 2, T_ADIAG_R);
        lcd_put_uint8(read_adc_8b(1));
        lcd_set_cursor(13,2);
        lcd_put_uint8(calval_rl);
        lcd_set_cursor(17,2);
        lcd_put_uint8(calval_rh);

        if (kbstate==K_ESC) break;
    }
}

void warn_settings()
{
	lcd_clear();
	lcd_mvputs( 0, 1, T_WARN_SETTINGS);

	while(1)
	{
		if(eeprom_read_byte(WARN_SETTING_ADDR)) lcd_mvputs( 0, 1, T_YES);
		else lcd_mvputs( 0, 1, T_NO);

		switch(wait_key())
		{
		case K_UP: eeprom_update_byte(WARN_SETTING_ADDR, 1); break;
		case K_DN: eeprom_update_byte(WARN_SETTING_ADDR, 0); break;
		case K_ESC: return;
		default: break;
		}

	}
}

void analog_cal()
{
    uint8_t calval_l=0, calval_r = 0;

    lcd_clear();
    lcd_mvputs( 0, 0, T_USER_CLOSE);
    lcd_mvputs( 0, 1, T_USER_KEY);

    while(!kbstate)
    {
    	lcd_set_cursor(0,3);
    	lcd_put_uint8(read_adc_8b(0));
    	lcd_set_cursor(17,3);
    	lcd_put_uint8(read_adc_8b(1));
    }
    lcd_clear();

    calval_l = read_adc_8b(0);
    calval_r = read_adc_8b(1);

    if( calval_l < ACAL_LIMIT_LL || calval_r < ACAL_LIMIT_LL || calval_l > ACAL_LIMIT_LH || calval_r > ACAL_LIMIT_LH)
    {
    	lcd_mvputs( 0, 0, T_ERR);

    	if(calval_l < ACAL_LIMIT_LL || calval_r < ACAL_LIMIT_LL) lcd_mvputs( 0, 1, T_CAL_LOW1);
    	else lcd_mvputs( 0, 1, T_CAL_HIGH1);

    	lcd_mvputs( 0, 2, T_CAL_HIGH2);

    	if( calval_l < ACAL_LIMIT_LL || calval_l > ACAL_LIMIT_LH ) lcd_mvputs( 0, 3, T_L);
    	else lcd_mvputs( 0, 3, T_R);

    	wait_key();
    	return;
    }

    eeprom_update_byte(CAL_VAL_LL_ADDR, calval_l);
    eeprom_update_byte(CAL_VAL_RL_ADDR, calval_r);

    lcd_mvputs( 0, 0, T_USER_OPEN);
    lcd_mvputs( 0, 1, T_USER_KEY);
    _delay_ms(1000);

    while(!kbstate)
    {
    	lcd_set_cursor(0,3);
    	lcd_put_uint8(read_adc_8b(0));
    	lcd_set_cursor(17,3);
    	lcd_put_uint8(read_adc_8b(1));
    }
    lcd_clear();

    calval_l = read_adc_8b(0);
    calval_r = read_adc_8b(1);

    if( calval_l < ACAL_LIMIT_HL || calval_r < ACAL_LIMIT_HL || calval_l > ACAL_LIMIT_HH || calval_r > ACAL_LIMIT_HH)
    {
    	lcd_mvputs( 0, 0, T_ERR);

    	if(calval_l < ACAL_LIMIT_HL || calval_r < ACAL_LIMIT_HL) lcd_mvputs( 0, 1, T_CAL_LOW1);
    	else lcd_mvputs( 0, 1, T_CAL_HIGH1);

    	lcd_mvputs( 0, 2, T_CAL_LOW2);

    	if( calval_l < ACAL_LIMIT_HL || calval_l > ACAL_LIMIT_HH ) lcd_mvputs( 0, 3, T_L);
    	else lcd_mvputs( 0, 3, T_R);

    	wait_key();
    	return;
    }

    eeprom_update_byte(CAL_VAL_LH_ADDR, calval_l);
    eeprom_update_byte(CAL_VAL_RH_ADDR, calval_r);


    beep();
    lcd_clear();
    lcd_mvputs( 0, 0, T_CAL_F2);
    lcd_mvputs( 0, 1, T_CAL_F3);

    lcd_set_cursor(0,2);
    lcd_put_uint8(eeprom_read_byte(CAL_VAL_LL_ADDR));
    lcd_set_cursor(17,2);
    lcd_put_uint8(eeprom_read_byte(CAL_VAL_RL_ADDR));
    lcd_set_cursor(0,3);
    lcd_put_uint8(eeprom_read_byte(CAL_VAL_LH_ADDR));
    lcd_set_cursor(17,3);
    lcd_put_uint8(eeprom_read_byte(CAL_VAL_RH_ADDR));

    _delay_ms(1000);
    wait_key();
}
