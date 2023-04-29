/*
 * disp.c - Display something on the LCD of the M1s
 */


#include <stdint.h>

#include "delay.h"
#include "mmio.h"
#include "lcd.h"


static const void *pattern(void)
{
	static uint16_t fb[LCD_HEIGHT][LCD_WIDTH];
	unsigned x, y;

	for (y = 0; y != LCD_HEIGHT; y++)
		for (x = 0; x != LCD_WIDTH; x++) {
			unsigned band = y / 10;

			fb[y][x] = !band || band == 27 || x == y ?
			    lcd_rgb(255, 0, 0) :
			    band & 1 ? 0xffff : 0;
		}
	return fb;
}


static const void *image(void)
{
	static const uint16_t fb[] = {
#include "citrine.inc"
	};

	return fb;
}


int main(int argc, char **argv)
{
	mmio_init();

	lcd_init();
	lcd_on();

	lcd_update(argc == 1 ? pattern() : image(),
	    0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

	return 0;
}
