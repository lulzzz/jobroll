#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include "include/config.h"
#include "include/uart.h"

/* The UMSEL bit in USART Control and Status Register C (UCSRC)
 * selects between asynchronous and synchronous operation. Double
 * speed (Asynchronous mode only) is cont rolled by the U2X found in
 * the UCSRA Register. When using Synchronous mode (UMSEL = 1), the
 * Data Direction Register for the XCK pin (DDR_XCK) controls whether
 * the clock source is internal (Master mode) or external (Slave
 * mode). The XCK pin is only active when using Synchronous mode.
 */

/* UCSRA: USART Control and Status Register A
 * ------------------------------------------------------------
 * | 7     | 6     | 5     | 4    | 3    | 2    | 1    | 0    |
 * | RXC   | TXC   | UDRE  | FR   | DOR  | PE   | U2X  | MPCM |
 * ------------------------------------------------------------
 *
 * UCSRB: USART Control and Status Register B
 * -------------------------------------------------------------
 * | 7     | 6     | 5     | 4    | 3    | 2     | 1    | 0    |
 * | RXCIE | TXCIE | UDRIE | RXEN | TXEN | UCSZ2 | RXB8 | TXB8 |
 * -------------------------------------------------------------
 *
 * UCSRC: USART Control and Status Register C
 * --------------------------------------------------------------
 * | 7     | 6     | 5    | 4    | 3    | 2     | 1     | 0     |
 * | URSEL | UMSEL | UPM1 | UPM0 | USBS | UCSZ1 | UCSZ2 | UCPOL |
 * --------------------------------------------------------------
 *
 */

#define BAUD 9600UL

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)

static uint8_t uart_getc(void);
static void uart_putc(uint8_t);
static void uart_putstr(uint8_t *);
static uint8_t uart_start(void);

static void
uart_putc(uint8_t b)
{
  /* The UDRE Flag indicates if the transmit buffer (UDR) is ready to
   * receive new data. If UDRE is one, the buffer is empty, and
   * therefore ready to be written.
   */
  while (IS_UNSET(UCSRA, UDRE));
  UDR = b;
}

static uint8_t
uart_getc() {
  uint8_t i = 0;
  /* This flag bit is set when there are unread data in the receive
   * buffer and cleared when the receive buffer is empty (that is,
   * does not contain any unread data).
   */
  while (IS_UNSET(UCSRA, RXC)) {
    if (i > 250) {
      i = 0;
      return '.';
    }
    _delay_ms(4);
    i++;
  }
  return UDR;
}

static void
uart_putstr(uint8_t *data)
{
  while (*data) {
    uart_putc(*data);
    data++;
  }
  uart_putc('\n');
}

static uint8_t
uart_start()
{
  if (uart_getc() == 0x2a) {
    uart_getc();
    uart_putstr((uint8_t *)"*");
    while (uart_getc() != 0x2d);
    return 1;
  }
  return 0;
}


uint8_t
uart_send(uint8_t *data)
{
  uint8_t i = 0;

  while (!uart_start()) {
    if (i > 250) {
      i = 0;
      return 0;
    }
    _delay_ms(4);
    i++;
  }
  
  uart_putstr(data);
  
  return 1;
}

void
uart_init() {
  UBRRH = UBRR_VAL >> 8;
  UBRRL = UBRR_VAL & 0xff;
  UCSRC = (1 << URSEL) | (1 << UCSZ1)| (1 << UCSZ0);
  UCSRB |= (1 << RXEN) | (1 << TXEN);
}

