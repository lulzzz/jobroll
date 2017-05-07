#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include "include/config.h"
#include "include/mfrc522.h"
#include "include/lcd.h"
#include "include/uart.h"

static void loop(void);

static void
loop()
{
  uint8_t data[16];
  uint8_t print[16];
  uint8_t status;

  for (;;) {
    /* The PCD typically polls for PICCs in the field. This is done
     * with the REQA (Request Command, Type A). When a PICC is within
     * the operating range of the PCD and receives the REQA, any
     * MIFARE PICC returns the ATQA (Answer To Request)
     */
    status = mfrc522_request(PICC_REQA, data);

    if (status == STATUS_OK) {
      PORTC |= (1 << PC5);
      PORTC &= ~(1 << PC5);

      status = mfrc522_anticollision(data);
      if (status == STATUS_OK) {

          status = mfrc522_select(data);

          sprintf((char *)print, "- %x %x %x %x", data[0], data[1], data[2], data[3]);
          uart_send((uint8_t *)print);

          lcd_clear();
          lcd_setcursor(0, 2);
          lcd_string((char *)print);
          _delay_ms(2000);

          lcd_clear();
          sprintf((char *)print, "Size: %d", status);
          lcd_string((char *)print);
          _delay_ms(2000);
          lcd_clear();
      }
      else STATUS_LED_BLINK(PC7);

    } else STATUS_LED_BLINK(PC7);
  }
}

int main(void)
{
  DDRC |= 0xff;
  PORTC = 0;

  lcd_init();
  uart_init();
	mfrc522_init();
  loop();

	return 0;
}
