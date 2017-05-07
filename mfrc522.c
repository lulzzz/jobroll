#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "include/config.h"
#include "include/mfrc522.h"
#include "include/spi.h"
#include "include/lcd.h"

void
mfrc522_write(uint8_t addr, uint8_t val)
{
	PORTA &= ~(1 << PA7);
  spi_transceive((addr << 1) & 0x7e);
  spi_transceive(val);
	PORTA |= (1 << PA7);
}

void
mfrc522_writes(uint8_t addr, uint8_t *val, uint8_t len) {
  PORTA &= ~(1 << PA7);
  spi_transceive((addr << 1) & 0x7e);
  for (uint8_t i = 0; i < len; i++) spi_transceive(val[i]);
	PORTA |= (1 << PA7);
}

uint8_t
mfrc522_read(uint8_t addr)
{
  uint8_t val;
	PORTA &= ~(1 << PA7);
	spi_transceive(((addr << 1) & 0x7e) | 0x80);
  val = spi_transceive(0);
	PORTA |= (1 << PA7);
	return val;
}

void
mfrc522_bitmask_set(uint8_t reg, uint8_t mask)
{
 uint8_t val = mfrc522_read(reg);
 mfrc522_write(reg, val | mask);
}

void
mfrc522_bitmask_clear(uint8_t reg, uint8_t mask)
{
 uint8_t val = mfrc522_read(reg);
 mfrc522_write(reg, val & (~mask));
}

void
mfrc522_crc_calculate(uint8_t *data, uint8_t len, uint8_t *buffer)
{
  /* stop any active command */
  mfrc522_write(CommandReg, PCD_IDLE);
  /* clear the CRCIRq interrupt request bit */
  mfrc522_write(DivIrqReg, 0x04);
  /* flush buffer */
  mfrc522_write(FIFOLevelReg, 0x80);
  /* write data to the buffer */
  mfrc522_writes(FIFODataReg, data, len);

  /* execute the command */
  mfrc522_write(CommandReg, PCD_CALCCRC);

  uint8_t irq;
  for (uint16_t i = 2000; i >= 0; i--) {
    irq = mfrc522_read(DivIrqReg);
    if ((irq & 0x04) != 0) break;
  }

  mfrc522_write(CommandReg, PCD_IDLE);
  buffer[0] = mfrc522_read(CRCResultRegL);
  buffer[1] = mfrc522_read(CRCResultRegH);
}

uint8_t
mfrc522_transceive(uint8_t command, uint8_t wait,
                   uint8_t *tx, uint8_t tx_len, uint8_t *rx, uint8_t *rx_len)
{
  /* stop any active command */
  mfrc522_write(CommandReg, PCD_IDLE);
  /* clear all seven interrupt request bits */
  mfrc522_write(ComIrqReg, 0x7f);
  /* flush buffer */
  mfrc522_write(FIFOLevelReg, 0x80);
  /* write data to the buffer */
  mfrc522_writes(FIFODataReg, tx, tx_len);

  /* execute the command
   *
   * each transmit process must be started by setting the
   * BitFramingReg register’s StartSend bit to logic 1. This command
   * must be cleared by writing any command to the CommandReg register
   */
  mfrc522_write(CommandReg, command);
  if (command == PCD_TRANSCEIVE) mfrc522_bitmask_set(BitFramingReg, 0x80);

  uint16_t i;
  for (i = 2000; i > 0; i--) {
    uint8_t n = mfrc522_read(ComIrqReg);
    if (n & wait) break;
    if (n & 0x01) return STATUS_TIMEOUT;
  }

  mfrc522_bitmask_clear(BitFramingReg, 0x80);

  if (i == 0) return STATUS_TIMEOUT;
  uint8_t error = mfrc522_read(ErrorReg);
  if (error & 0x13) return STATUS_ERROR;

  /* number of bytes in the FIFO */
  *rx_len = mfrc522_read(FIFOLevelReg); 
  for (uint8_t i = 0; i < *rx_len; i++) {
    rx[i] = mfrc522_read(FIFODataReg);
  }
  
  return STATUS_OK;
}

/* use with PICC_REQA or PICC_WUPA */

uint8_t
mfrc522_request(uint8_t mode, uint8_t *data)
{
  uint8_t rx_len;
  data[0] = mode;

  /* for REQA and WUPA we need the short frame format - transmit only
   * 7 bits of the last (and only) byte */
  mfrc522_write(BitFramingReg, 0x07);

  /* all received bits will be cleared after a collision only used
   * during bitwise anticollision at 106 kBd, otherwise it is set to
   * logic 1 */
  mfrc522_bitmask_clear(CollReg, 0x80);

  return mfrc522_transceive(PCD_TRANSCEIVE, 0x30, data, 1, data, &rx_len);
}

uint8_t
mfrc522_anticollision(uint8_t *uid)
{
  uint8_t uid_len;
  /* The PCD shall assign SEL with the code 
   * for the selected anticollision cascade level */
  uid[0] = PICC_ANTICOLL;

  /* the PCD shall assign NVB with the value of '20'
   *
   * this value defines that the PCD will transmit no part of UID CLn
   *
   * consequently this command forces all PICCs in the field to
   * respond with their complete UID CLn
   */
  uid[1] = 0x20;
  mfrc522_write(BitFramingReg, 0);

  /* the PCD shall transmit SEL and NVB */
  return mfrc522_transceive(PCD_TRANSCEIVE, 0x30, uid, 2, uid, &uid_len);
}

uint8_t
mfrc522_select(uint8_t *uid)
{
  /* Byte 0: SEL
   * Byte 1: NVB
   * Byte 2: UID
   * Byte 3: UID
   * Byte 4: UID
   * Byte 5: UID
   * Byte 6: BCC
   * Byte 7: CRC
   * Byte 8: CRC
   */

  uint8_t status;
  uint8_t rx_len;
  uint8_t buffer[9];

  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;

  for (uint8_t i = 0; i < 4; i++) buffer[i + 2] = *(uid + i);

  buffer[6] = (buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5]) & 0xff;
  mfrc522_crc_calculate(buffer, 7, &buffer[7]);

  status = mfrc522_transceive(PCD_TRANSCEIVE, 0x30, buffer, 9, buffer, &rx_len);

  if (status == STATUS_OK) return buffer[0];
  else return 0;
}

void
mfrc522_reset()
{
	mfrc522_write(CommandReg, PCD_SOFTRESET);
  while (mfrc522_read(CommandReg) & (1 << 4));

  mfrc522_write(TModeReg, 0x80);
  mfrc522_write(TPrescalerReg, 0xff);

  mfrc522_write(TxASKReg, 0x40);
  mfrc522_write(ModeReg, 0x3d);
}

void
mfrc522_init()
{
  uint8_t val;

	DDRA  |= (1 << PA7);
	PORTA |= (1 << PA7);
	
	spi_init();	
	mfrc522_reset();

	val = mfrc522_read(VersionReg);
	if ((val & 1) == 1 || (val & 2) == 2) STATUS_LED_BLINK(0)

  mfrc522_write(TReloadRegL, 0xe8);
  mfrc522_write(TReloadRegH, 0x03);

  /* Antenna ON, TX1 and TX2 */
  mfrc522_bitmask_set(TxControlReg, 3);

  /* Antenna OFF
   * mfrc522_bitmask_clear(TxControlReg, 3);
  */
}
