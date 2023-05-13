/*
 * disp.c - Display something on the LCD of the M1s
 */


#include <stdint.h>

#include "delay.h"
#include "mmio.h"
#include "spi.h"
#include "st7789.h"
#include "bl.h"


/* --- M1s LCD connections ------------------------------------------------- */

#define LCD_CS		12	/* IO12_LCD_DBI_CS */
#define LCD_DnC		13	/* IO13_LCD_DBI_DC */
#define LCD_MOSI	25	/* IO25_LCD_DBI_SDA */
#define LCD_SCLK	19	/* IO19_LCD_DBI_SCK */

#define LCD_RST		24	/* IO24_LCD_RESET */

#define LCD_BL		11	/* IO11_LCD_BL_PWM */

#define	LCD_SPI		0

#define LCD_WIDTH	240
#define LCD_HEIGHT	280


static const void *pattern(void)
{
	static uint16_t fb[LCD_HEIGHT][LCD_WIDTH];
	unsigned x, y;

	for (y = 0; y != LCD_HEIGHT; y++)
		for (x = 0; x != LCD_WIDTH; x++) {
			unsigned band = y / 10;

			fb[y][x] = !band || band == 27 || x == y ?
			    st7789_rgb(255, 0, 0) :
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

	/*
	 * ST7789V can go as fast as 1 / 66 ns = 15.15 MHz
	 */
	spi_init(LCD_MOSI, LCD_SCLK, LCD_CS, 15);
	bl_init(LCD_BL);
	st7789_init(LCD_SPI, LCD_RST, LCD_DnC, LCD_WIDTH, LCD_HEIGHT, 0, 20);
	st7789_on();
	bl_on(1);

	st7789_update(argc == 1 ? pattern() : image(),
	    0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

	return 0;
}
