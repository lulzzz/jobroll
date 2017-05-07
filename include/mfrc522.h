/* The MFRC522 is a highly integrated reader/writer IC for contactless
 * communication at 13.56 MHz */

#ifndef MFRC522_H_
#define MFRC522_H_

/* Proximity Coupling Device
 * PCD: MFRC522 (Contactless Reader)
 */

/* no action, cancels current command execution */
#define PCD_IDLE              0x00
/* stores 25 bytes into the internal buffer */
#define PCD_MEM               0x01
/* generates a 10-byte random ID number */
#define PCD_RANDOMID          0x02
/* activates the CRC coprocessor or performs a self test */
#define PCD_CALCCRC           0x03
/* transmits data from the FIFO buffer */
#define PCD_TRANSMIT          0x04
/* no command change, can be used to modify the CommandReg register
 * bits without affecting the command, for example, the PowerDown bit */
#define PCD_NOCMDCHANGE       0x07
/* activates the receiver circuits */
#define PCD_RECEIVE           0x08
/* transmits data from FIFO buffer to antenna and automatically
 * activates the receiver after transmission */
#define PCD_TRANSCEIVE        0x0C
/* performs the MIFARE standard authentication as a reader */
#define PCD_MFAUTHENT         0x0E
/* resets the MFRC522*/
#define PCD_SOFTRESET         0x0F

/* Proximity Integrated Circuit
 * PICC: Contactless Card
 */

/* Invites PICCs in state IDLE to go to READY and prepare for
 * anticollision or selection. 7 bit frame */
#define PICC_REQA             0x26
#define PICC_HALT             0x50
/* Invites PICCs in state IDLE and HALT to go to READY(*) and prepare
 * for anticollision or selection. 7 bit frame */
#define PICC_WUPA             0x52               //Search all the cards in the antenna area
#define PICC_ANTICOLL         0x93               //prevent conflict
#define PICC_SElECTTAG        0x93               //select card
#define PICC_ANTICOLL2        0x95				// anticollision level 2
#define PICC_ANTICOLL3        0x97               // anticollision level 3
#define PICC_AUTHENT1A        0x60               //verify A password key
#define PICC_AUTHENT1B        0x61               //verify B password key
#define PICC_READ             0x30               //read
#define PICC_WRITE            0xA0               //write
#define PICC_DECREMENT        0xC0               //deduct value
#define PICC_INCREMENT        0xC1               //charge up value
#define PICC_RESTORE          0xC2               //Restore data into buffer
#define PICC_TRANSFER         0xB0               //Save data into buffer

#define     Reserved00        0x00
#define     CommandReg        0x01
#define     ComIEnReg         0x02
#define     DivlEnReg         0x03
#define     ComIrqReg         0x04
#define     DivIrqReg         0x05
#define     ErrorReg          0x06
#define     Status1Reg        0x07
#define     Status2Reg        0x08
#define     FIFODataReg       0x09
#define     FIFOLevelReg      0x0A
#define     WaterLevelReg     0x0B
#define     ControlReg        0x0C
#define     BitFramingReg     0x0D
#define     CollReg           0x0E
#define     Reserved01        0x0F
#define     Reserved10        0x10
#define     ModeReg           0x11
#define     TxModeReg         0x12
#define     RxModeReg         0x13
#define     TxControlReg      0x14
#define     TxASKReg          0x15
#define     TxSelReg          0x16
#define     RxSelReg          0x17
#define     RxThresholdReg    0x18
#define     DemodReg          0x19
#define     Reserved11        0x1A
#define     Reserved12        0x1B
#define     MifareReg         0x1C
#define     Reserved13        0x1D
#define     Reserved14        0x1E
#define     SerialSpeedReg    0x1F
#define     Reserved20        0x20
#define     CRCResultRegH     0x21
#define     CRCResultRegL     0x22
#define     Reserved21        0x23
#define     ModWidthReg       0x24
#define     Reserved22        0x25
#define     RFCfgReg          0x26
#define     GsNReg            0x27
#define     CWGsPReg	        0x28
#define     ModGsPReg         0x29
#define     TModeReg          0x2A
#define     TPrescalerReg     0x2B
#define     TReloadRegH       0x2C
#define     TReloadRegL       0x2D
#define     TCounterValueRegH 0x2E
#define     TCounterValueRegL 0x2F
#define     Reserved30        0x30
#define     TestSel1Reg       0x31
#define     TestSel2Reg       0x32
#define     TestPinEnReg      0x33
#define     TestPinValueReg   0x34
#define     TestBusReg        0x35
#define     AutoTestReg       0x36
#define     VersionReg        0x37
#define     AnalogTestReg     0x38
#define     TestDAC1Reg       0x39
#define     TestDAC2Reg       0x3A
#define     TestADCReg        0x3B
#define     Reserved31        0x3C
#define     Reserved32        0x3D
#define     Reserved33        0x3E
#define     Reserved34			  0x3F

void mfrc522_init(void);
void mfrc522_write(uint8_t, uint8_t);
void mfrc522_writes(uint8_t, uint8_t *, uint8_t);
uint8_t mfrc522_read(uint8_t addr);
void mfrc522_reset(void);
void mfrc522_bitmask_set(uint8_t, uint8_t);
void mfrc522_bitmask_clear(uint8_t, uint8_t);
uint8_t mfrc522_request(uint8_t, uint8_t *);
uint8_t mfrc522_transceive(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t *, uint8_t *);
uint8_t mfrc522_anticollision(uint8_t *);
void mfrc522_crc_calculate(uint8_t *, uint8_t, uint8_t *);
uint8_t mfrc522_select(uint8_t *);

#endif /* MFRC522_H_ */
