/*
 * st7789.h - Driver for the ST7789V TFT controller
 */

#ifndef ST7789_H
#define	ST7789_H

#include <stdint.h>


/* R5 G6 B5 */

static inline uint16_t st7789_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r & 0xf8) | (g & 0xe0) >> 5 | (g & 0x1c) << 6 | (b & 0xf8) >> 3;
//	return (r & 0xf8) << 8 | (g & 0xfc) << 3 | (b & 0xf8) >> 3;
}


void st7789_update(const void *fb, unsigned x0, unsigned y0, unsigned x1,
    unsigned y1);
void st7789_on(void);
void st7789_init(unsigned spi, unsigned rst, unsigned dnc,
    unsigned width, unsigned heigth, unsigned xoff, unsigned yoff);

#endif /* !ST7789_H */
