/*
 * lcd.h - LCD driver (ST7789V on M1s)
 */

#ifndef LCD_H
#define	LCD_H

#include <stdint.h>


#define	LCD_WIDTH	240
#define	LCD_HEIGHT	280


/* R5 G6 B5 */

static inline uint16_t lcd_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r & 0xf8) | (g & 0xe0) >> 5 | (g & 0x1c) << 6 | (b & 0xf8) >> 3;
//	return (r & 0xf8) << 8 | (g & 0xfc) << 3 | (b & 0xf8) >> 3;
}


void lcd_update(const void *fb, unsigned x0, unsigned y0, unsigned x1,
    unsigned y1);
void lcd_on(void);
void lcd_init(void);

#endif /* !LCD_H */
