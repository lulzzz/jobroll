#include <avr/io.h>
#include "include/config.h"

uint8_t
spi_transceive(uint8_t data)
{
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  return SPDR; 
}

void
spi_init()
{
  /* Set SS, MOSI, SCK output */
  DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB7);

  /* Enable SPI, Master, set clock rate fck/16 */
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

