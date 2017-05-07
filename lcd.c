#include <avr/io.h>
#include <util/delay.h>

#include "include/config.h"
#include "include/lcd.h"
#include "include/hd44780.h"

static void lcd_enable(void);
static void lcd_out(uint8_t);
static void lcd_soft_reset(void);

static void
lcd_enable()
{
  LCD_PORT |= (1 << LCD_E);
  _delay_us(LCD_ENABLE_US);
  LCD_PORT &= ~(1 << LCD_E);
}
 
static void
lcd_out(uint8_t data)
{
  data &= 0xf0;
  PORTD &= 0x0f;
  PORTD |= (data & 0xf0);
  lcd_enable(); 
}

static void
lcd_soft_reset()
{
  lcd_out(LCD_SOFT_RESET);
  _delay_ms(LCD_SOFT_RESET_MS1);
  lcd_enable();
  _delay_ms(LCD_SOFT_RESET_MS2); 
  lcd_enable();
  _delay_ms( LCD_SOFT_RESET_MS3 );
}

void
lcd_on()
{
  lcd_command( LCD_SET_DISPLAY
             | LCD_DISPLAY_ON
             | LCD_CURSOR_OFF
             | LCD_BLINKING_OFF);

  lcd_clear();
}

void
lcd_off()
{
  lcd_command( LCD_SET_DISPLAY
             | LCD_DISPLAY_OFF
             | LCD_CURSOR_OFF
             | LCD_BLINKING_OFF);

  lcd_clear();
}

void
lcd_init()
{
    LCD_DDR |= (1 << LCD_R) | (1 << LCD_E);
    DDRD |= 0xf0;
    _delay_ms( LCD_BOOTUP_MS );
    
    lcd_soft_reset();
 
    /* 4-bit mode */
    lcd_out(LCD_SET_FUNCTION | LCD_FUNCTION_4BIT);
    _delay_ms(LCD_SET_4BITMODE_MS);
 
    /* 4-bit mode / 2 lines / 5x7 */
    lcd_command( LCD_SET_FUNCTION
               | LCD_FUNCTION_4BIT
               | LCD_FUNCTION_2LINE
               | LCD_FUNCTION_5X7);

    lcd_command( LCD_SET_ENTRY
               | LCD_ENTRY_INCREASE
               | LCD_ENTRY_NOSHIFT);
    
    lcd_on();
    lcd_clear();
}
  
void 
lcd_data(uint8_t data)
{
  LCD_PORT |= (1 << LCD_R);
  lcd_out(data);
  lcd_out(data << 4);
  _delay_us(LCD_WRITEDATA_US);
}
 
void
lcd_command(uint8_t data)
{
  LCD_PORT &= ~(1 << LCD_R);
  lcd_out(data);
  lcd_out(data << 4);
  _delay_us(LCD_COMMAND_US);
}
 
void
lcd_clear()
{
  lcd_command(LCD_CLEAR_DISPLAY);
  _delay_ms(LCD_CLEAR_DISPLAY_MS);
}
 
void
lcd_home()
{
  lcd_command(LCD_CURSOR_HOME);
  _delay_ms(LCD_CURSOR_HOME_MS);
}
 
void
lcd_setcursor(uint8_t x, uint8_t y)
{
  uint8_t data;
  switch (y) {
    case 1:
      data = LCD_SET_DDADR + LCD_DDADR_LINE1 + x;
      break;
    case 2:
      data = LCD_SET_DDADR + LCD_DDADR_LINE2 + x;
      break;
    case 3:
      data = LCD_SET_DDADR + LCD_DDADR_LINE3 + x;
      break;
    case 4:
      data = LCD_SET_DDADR + LCD_DDADR_LINE4 + x;
      break;
    default:
      return;
  }
 
  lcd_command(data);
}
 
void
lcd_string(const char *str)
{
  while (*str != '\0') {
    lcd_data(*str++);
  }
}
