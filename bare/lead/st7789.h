/*
 * st7789.h - Driver for the ST7789V TFT controller
 */

#ifndef ST7789_H
#define	ST7789_H

#include <stdint.h>


/*
 * R5 G6 B5, with byte swapping.
 *
 * RGB888:			 RRRRRxxx GGGgggxx BBBBBxxx
 *					     |
 * RGB88 and byte-swapped:		     v
 * RRRR.RGGG.gggB.BBBBB becomes gggB.BBBBB.RRRR.RGGG
 */

static inline uint16_t st7789_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r & 0xf8) | (g & 0xe0) >> 5 | (g & 0x1c) << 12 |
	    (b & 0xf8) << 5;
}


void st7789_update(const void *fb, unsigned x0, unsigned y0, unsigned x1,
    unsigned y1);
void st7789_on(void);
void st7789_init(unsigned spi, unsigned rst, unsigned dnc,
    unsigned width, unsigned heigth, unsigned xoff, unsigned yoff);

#endif /* !ST7789_H */
