
#ifndef MAX7219_H_8X8
#define MAX7219_H_8X8

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#ifndef	MAX7219_DIN_PIN_8X8
# define	MAX7219_DIN_PIN_8X8		PB2
#endif	/* !MAX7219_DIN_PIN */

#ifndef	MAX7219_CLK_PIN_8X8
# define	MAX7219_CLK_PIN_8X8		PB0
#endif	/* !MAX7219_CLK_PIN */

#ifndef	MAX7219_CS_PIN_8X8
# define	MAX7219_CS_PIN_8X8		PB1
#endif	/* !MAX7219_CS_PIN */

/**
 * Initialize display driver.
 * Clock pin, data pin and chip select pin
 * are defined at the top of this file.
 */
void MAX7219_init_8X8(void);

/**
 * Set status of a single pixel.
 * @param row: row number from range <0, 7>
 * @param col: column number from range <0, 7>
 */
void MAX7219_set_pixel_8X8(uint8_t row, uint8_t col, bool value);

/**
 * Set all 8 LEDs in a row.
 * @param row: row number from range <0, 7>
 * @param value: row value (each bit represents a LED in a row, i.e. 0b00001111)
 */
void MAX7219_set_row_8X8(uint8_t row, uint8_t value);

/**
 * Set all 8 LEDs in a column.
 * @param column: column number from range <0, 7>
 * @param value: column value (each bit represents a LED in a column, i.e. 0b00001111)
 */
void MAX7219_set_column_8X8(uint8_t column, uint8_t value);

/**
 * Set brightness of the display.
 * @param value: intensity from range <0, 15>
 */
void MAX7219_set_intensity_8X8(uint8_t value);

/**
 * Clear display. 
 */
void MAX7219_clear_8X8(void);

#define MAX7219_REG_NOOP                (0x00)
#define MAX7219_REG_DIGIT0              (0x01)
#define MAX7219_REG_DIGIT1              (0x02)
#define MAX7219_REG_DIGIT2              (0x03)
#define MAX7219_REG_DIGIT3              (0x04)
#define MAX7219_REG_DIGIT4              (0x05)
#define MAX7219_REG_DIGIT5              (0x06)
#define MAX7219_REG_DIGIT6          (0x07)
#define MAX7219_REG_DIGIT7          (0x08)
#define MAX7219_REG_DECODEMODE		(0x09)
#define MAX7219_REG_INTENSITY		(0x0A)
#define MAX7219_REG_SCANLIMIT		(0x0B)
#define MAX7219_REG_SHUTDOWN		(0x0C)
#define MAX7219_REG_DISPLAYTEST		(0x0F)

#define MAX7219_DIN_HIGH1()              (PORTB |= _BV(MAX7219_DIN_PIN_8X8))
#define MAX7219_DIN_LOW1()               (PORTB &= ~_BV(MAX7219_DIN_PIN_8X8))
#define MAX7219_CLK_HIGH1()              (PORTB |= _BV(MAX7219_CLK_PIN_8X8))
#define MAX7219_CLK_LOW1()               (PORTB &= ~_BV(MAX7219_CLK_PIN_8X8))
#define MAX7219_CS_HIGH1()               (PORTB |= _BV(MAX7219_CS_PIN_8X8))
#define MAX7219_CS_LOW1()                (PORTB &= ~_BV(MAX7219_CS_PIN_8X8))

static uint8_t MAX7219_state[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static void MAX7219_write_8X8(uint8_t value)
{
	uint8_t i;

	__asm("nop");
	for (i = 0; i < 8; ++i, value <<= 1) {
		MAX7219_CLK_LOW1();
		__asm("nop");
		if (value & 0x80) {
			MAX7219_DIN_HIGH1();
			} else {
			MAX7219_DIN_LOW1();
		}
		MAX7219_CLK_HIGH1();
	}
}

static void MAX7219_send_8X8(uint8_t reg, uint8_t data)
{

	MAX7219_CS_HIGH1();
	MAX7219_write_8X8(reg);
	MAX7219_write_8X8(data);
	MAX7219_CS_LOW1();
	__asm("nop");
	MAX7219_CS_HIGH1();
}

void MAX7219_init_8X8(void)
{

	DDRB |= _BV(MAX7219_DIN_PIN_8X8)|_BV(MAX7219_CLK_PIN_8X8)|_BV(MAX7219_CS_PIN_8X8);
	MAX7219_send_8X8(MAX7219_REG_DECODEMODE, 0x00);
	MAX7219_send_8X8(MAX7219_REG_SCANLIMIT, 0x07);
	MAX7219_send_8X8(MAX7219_REG_INTENSITY, 0x0f);
	MAX7219_send_8X8(MAX7219_REG_DISPLAYTEST, 0x00);
	MAX7219_send_8X8(MAX7219_REG_SHUTDOWN, 0x01);
}

void MAX7219_set_pixel_8X8(uint8_t row, uint8_t col, bool value)
{
	uint8_t data;

	if (row > 7 || col > 7)
	return;

	data = 1 << col;
	if (value) {
		MAX7219_state[row] |= data;
		} else {
		MAX7219_state[row] &= ~data;
	}

	MAX7219_send_8X8(row + 1, MAX7219_state[row]);
}

void MAX7219_set_row_8X8(uint8_t row, uint8_t value)
{

	if (row > 7)
	return;

	MAX7219_state[row] = value;
	MAX7219_send_8X8(row + 1, value);
}

void MAX7219_set_column_8X8(uint8_t column, uint8_t value)
{
	uint8_t i;

	if (column > 7)
	return;

	for (i = 0; i < 8; ++i) {
		MAX7219_set_pixel_8X8(i, column, !!((value >> i) & 0x01));
	}
}

void MAX7219_set_intensity_8X8(uint8_t value)
{

	if (value > 15)
	return;

	MAX7219_send_8X8(MAX7219_REG_INTENSITY, value);
}

void MAX7219_clear_8X8(void)
{
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		if (MAX7219_state[i]) {
			MAX7219_state[i] = 0;
			MAX7219_send_8X8(i + 1, 0);
		}
	}
}

#endif  /* !MAX7219_H_8X8 */
