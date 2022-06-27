#pragma once

#include "lcd.h"
#include "text.h"
#include "keypad.h"
#include "adc.h"
#include <avr/eeprom.h>

#define PACKER_V 60700
#define PACKER_A 231

//16/36 ratio
//#define PLANTER_V 12676056
//#define PLANTER_A 142
//#define PLANTER_CAL_NO {121,128,135}

//15/36 ratio
#define PLANTER_V 13501350
#define PLANTER_A 133
#define PLANTER_CAL_NO {113,120,126}

#define MODE_HARROW 2
#define MODE_PLANTER 3
#define MODE_COMBI 1
#define MODE_SERVICE 5
#define MODE_PLANTER_CAL 4

#define ERR_STOP 0x10
#define ERR_PLANTER_STOP 0x10
#define ERR_PACKER_STOP 0x11

#define ERR_LOW_LEVEL 0x20
#define ERR_LOW_LEVEL_L 0x21
#define ERR_LOW_LEVEL_R 0x22

#define ERR_SLIP 0x30
#define ERR_PLANTER_SLIP 0x30
#define ERR_PACKER_SLIP 0x31

#define R_FAIL 0x02
#define L_FAIL 0x01
#define SLIP_FAIL 0x04

#define CAL_STATUS_ADDR (uint8_t*)0x00

#define CAL_VAL_LL_ADDR (uint8_t*)0x01
#define CAL_VAL_RL_ADDR (uint8_t*)0x02
#define CAL_VAL_LH_ADDR (uint8_t*)0x03
#define CAL_VAL_RH_ADDR (uint8_t*)0x04

#define WARN_SETTING_ADDR (uint8_t*)0x05

#define MAIN_MENU_ENTRIES {T_COMBI,T_HARROW,T_PLANTER,T_PLANTER_CAL,T_SERVICE_MENU}
#define MAIN_MENU_ENTRIES_NO 5

#define SERVICE_MENU_ENTRIES {T_SPEED_DIAG,T_ANALOG_DIAG,T_SERV_CAL,T_WARN_SETTINGS}
#define SERVICE_MENU_ENTRIES_NO 4

#define ACAL_LIMIT_LL 5
#define ACAL_LIMIT_LH 80
#define ACAL_LIMIT_HL 200
#define ACAL_LIMIT_HH 240

#define SLIP_RATIO 2
#define LEVEL_SAFETY_RESET 15
#define LEVEL_SAFETY_LOW 3

#define ICIE 5

uint8_t check( uint8_t speedcheck, uint8_t slip_flag, uint8_t level_l, uint8_t level_r );
uint8_t handle_error(uint8_t errno);
void throw_error( const char* msg );
void throw_warn( const char* msg_line1, const char* msg_line2 );

void print_data(uint16_t area, uint16_t speed);
void set_back_text();
void welcome_screen();
void lcd_start();
void init_sensors();

uint8_t menu( const char* entries[], const uint8_t entries_no );

void run_planter(uint8_t speedcheck);
void run_harrow();
void planter_cal();

void speed_diag();
void analog_diag();
void analog_cal();
void warn_settings();

uint8_t cal_values( uint8_t* addr_0, uint8_t* addr_1, uint8_t hi_lo );
void check_cal();
