#include "lcd.h"

#include <util/delay.h>

void lcd_send(uint8_t value, uint8_t mode);
void lcd_write_nibble(uint8_t nibble);

static uint8_t lcd_displayparams;

void lcd_command(uint8_t command) {
  lcd_send(command, 0);
}

void lcd_write(uint8_t value) {
  lcd_send(value, 1);
}

void lcd_send(uint8_t value, uint8_t mode) {
  if (mode) {
    LCD_PORT = LCD_PORT | (1 << LCD_RS);
  } else {
    LCD_PORT = LCD_PORT & ~(1 << LCD_RS);
  }

  //LCD_PORT = LCD_PORT & ~(1 << LCD_RW);

  lcd_write_nibble(value >> 4);
  lcd_write_nibble(value);
}

void lcd_write_nibble(uint8_t nibble) {
  LCD_PORT = (LCD_PORT & 0xff & ~(0x0f << LCD_D0)) | ((nibble & 0x0f) << LCD_D0);

  LCD_PORT = LCD_PORT & ~(1 << LCD_EN);
  LCD_PORT = LCD_PORT | (1 << LCD_EN);
  LCD_PORT = LCD_PORT & ~(1 << LCD_EN);
  _delay_us(300);
}

void lcd_init(void) {
  // Configure pins as output
  LCD_DDR = LCD_DDR
    | (1 << LCD_RS)
    //| (1 << LCD_RW)
    | (1 << LCD_EN)
    | (1 << LCD_D0)
    | (1 << LCD_D1)
    | (1 << LCD_D2)
    | (1 << LCD_D3);

  // Wait for LCD to become ready (docs say 15ms+)
  _delay_ms(15);

  LCD_PORT = LCD_PORT
    & ~(1 << LCD_EN)
    & ~(1 << LCD_RS);
    //& ~(1 << LCD_RW);

  _delay_ms(4.1);

  lcd_write_nibble(0x03); // Switch to 4 bit mode
  _delay_ms(4.1);

  lcd_write_nibble(0x03); // 2nd time
  _delay_ms(4.1);

  lcd_write_nibble(0x03); // 3rd time
  _delay_ms(4.1);

  lcd_write_nibble(0x02); // Set 8-bit mode (?)

  lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);

  lcd_displayparams = LCD_CURSOROFF | LCD_BLINKOFF;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_on(void) {
  lcd_displayparams |= LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_off(void) {
  lcd_displayparams &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_clear(void) {
  lcd_command(LCD_CLEARDISPLAY);
  _delay_ms(2);
}

void lcd_return_home(void) {
  lcd_command(LCD_RETURNHOME);
  _delay_ms(2);
}

void lcd_enable_blinking(void) {
  lcd_displayparams |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_blinking(void) {
  lcd_displayparams &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_enable_cursor(void) {
  lcd_displayparams |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_disable_cursor(void) {
  lcd_displayparams &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | lcd_displayparams);
}

void lcd_scroll_left(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scroll_right(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcd_set_left_to_right(void) {
  lcd_displayparams |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_set_right_to_left(void) {
  lcd_displayparams &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_enable_autoscroll(void) {
  lcd_displayparams |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_disable_autoscroll(void) {
  lcd_displayparams &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | lcd_displayparams);
}

void lcd_create_char(uint8_t location, uint8_t *charmap) {
  lcd_command(LCD_SETCGRAMADDR | ((location & 0x7) << 3));
  for (int i = 0; i < 8; ++i) {
    lcd_write(charmap[i]);
  }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };

  if (row > 3) {
    row = 3;
  }

  lcd_command(LCD_SETDDRAMADDR | (col + offsets[row]));
}

void lcd_mvputs( uint8_t col, uint8_t row, const char *string)
{
	static uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	lcd_command(LCD_SETDDRAMADDR | (col + offsets[row]));

	for ( char* c = (char*) string; *c; ++c) lcd_write(*c);
}

void lcd_put_uint8( uint8_t num )
{
    uint8_t sing=0, ten=0, hund=0;
    hund = num/100;
    lcd_write( hund==0 ? ' ':hund+0x30 );
    ten = (num%100) / 10;
    lcd_write( ten==0&&hund==0 ? ' ':ten+0x30 );
    sing = num%10;
    lcd_write(sing+0x30);
}

void lcd_put_uint32( uint32_t num )
{
	uint8_t out[10];
	uint8_t i = 10;

	do
	{
		out[i-1] = num%10;
		num /= 10;
		--i;
	}
	while( num );

	for( uint8_t j = i; j < 10; ++j) lcd_write( out[j]+0x30);
}

void lcd_put_f( uint16_t num )
{
    uint8_t sing=0, ten=0, hund=0, one_below=0, two_below=0;

    two_below = num%10;
    one_below = (num%100)/10;
    hund = num/10000;
    lcd_write( hund==0 ? ' ':hund+0x30 );
    ten = (num%10000) / 1000;
    lcd_write( ten==0&&hund==0 ? ' ':ten+0x30 );
    sing = (num%1000) / 100;
    lcd_write(sing+0x30);
    lcd_write('.');
    lcd_write(one_below+0x30);
    lcd_write(two_below+0x30);
}


void lcd_bar( uint8_t length, uint8_t line)
{
    lcd_set_cursor(0,line);
    length = (length<=LCD_COL_COUNT ? length : LCD_COL_COUNT);
    for( uint8_t size=length ; size>0 ; --size) lcd_write(0xFF);
    for( uint8_t size=LCD_COL_COUNT-length ; size>0 ; --size) lcd_write(0x5F);
}
