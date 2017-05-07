#ifndef CONFIG_H_
#define CONFIG_H_

#define LED_DDR DDRC
#define LED_OUTPUT PORTC

#define LCD_PORT PORTA
#define LCD_DDR  DDRA
#define LCD_R    PA0
#define LCD_E    PA1

#define STATUS_LED_BLINK(pin) LED_DDR |= (1 << pin); \
                         LED_OUTPUT |= (1 << pin); \
                         _delay_ms(500); \
                         LED_OUTPUT &= ~(1 << pin); \
                         _delay_ms(500);

#define STATUS_OK             0
#define STATUS_ERROR          1
#define STATUS_COLLISION      2
#define STATUS_TIMEOUT        3
#define STATUS_NO_ROOM        4
#define STATUS_INTERNAL_ERROR 5
#define STATUS_INVALID        6
#define STATUS_CRC_WRONG      7

#endif /* CONFIG_H_ */
