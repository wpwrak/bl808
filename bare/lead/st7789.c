/*
 * st7789.c - Driver for the ST7789V TFT controller
 */


#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "spi.h"
#include "delay.h"
#include "st7789.h"


static unsigned st7789_spi;
static unsigned st7789_dnc;
static unsigned	st7789_width;
static unsigned	st7789_heigth;
static unsigned	st7789_xoff;
static unsigned	st7789_yoff;


/* --- ST7789V commands ---------------------------------------------------- */

#define	ST7789_SLPOUT	0x11
#define	ST7789_NORON	0x13
#define	ST7789_INVON	0x21
#define	ST7789_DISPON	0x29
#define	ST7789_CASET	0x2a
#define	ST7789_RASET	0x2b
#define	ST7789_RAMWR	0x2c
#define	ST7789_MADCTL	0x36
#define	ST7789_COLMOD	0x3a


/* --- Send commands and parameters ---------------------------------------- */


static void st7789_cmd(uint8_t cmd)
{
	spi_start();
	gpio_out(st7789_dnc, 0);
	spi_send(&cmd, 1);
	spi_end();
}


static void st7789_cmd8(uint8_t cmd, uint8_t value)
{
	spi_start();
	gpio_out(st7789_dnc, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(st7789_dnc, 1);
	spi_send(&value, 1);
	spi_end();
}


static void st7789_cmd32(uint8_t cmd, uint32_t value)
{
	uint8_t tmp[] = { value >> 24, value >> 16, value >> 8, value };

	spi_start();
	gpio_out(st7789_dnc, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(st7789_dnc, 1);
	spi_send(tmp, 4);
	spi_end();
}


static void st7789_send(uint8_t cmd, const void *buf, unsigned len)
{
	spi_start();
	gpio_out(st7789_dnc, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(st7789_dnc, 1);
	spi_send(buf, len);
	spi_end();
}


/* --- API ----------------------------------------------------------------- */


void st7789_update(const void *fb, unsigned x0, unsigned y0, unsigned x1,
    unsigned y1)
{
	st7789_cmd32(ST7789_CASET,
	    (x0 + st7789_xoff) << 16 | (x1 + st7789_xoff));
	/* @@@ no idea why we need this offset, but we do ... */
	st7789_cmd32(ST7789_RASET,
	    (y0 + st7789_yoff) << 16 | (y1 + st7789_yoff));

	st7789_send(ST7789_RAMWR, fb, (x1 - x0 + 1) * (y1 - y0 + 1) * 2);

	/* @@@ we need a small delay between frames */
	mdelay(1);
}


void st7789_on(void)
{
	st7789_cmd(ST7789_DISPON);
	mdelay(1);
}


void st7789_init(unsigned spi, unsigned rst, unsigned dnc,
    unsigned width, unsigned heigth, unsigned xoff, unsigned yoff)
{
	st7789_spi = spi;
	st7789_dnc = dnc;
	st7789_width = width;
	st7789_heigth = heigth;
	st7789_xoff = xoff;
	st7789_yoff = yoff;

	gpio_cfg_out(rst, 1, 0);
	gpio_cfg_out(dnc, 1, 0);

	/* --- reset and wakeup --- */

	gpio_out(rst, 0);
	mdelay(1);
	gpio_out(rst, 1);
	mdelay(120);

	st7789_cmd(ST7789_SLPOUT);		// exit sleep mode
	mdelay(120);				// SLPOUT-SLPIN timing

	/* --- display organization --- */

	st7789_cmd8(ST7789_COLMOD, 5 << 4 | 5);	// pixel format
	st7789_cmd8(ST7789_MADCTL, 1 << 7 | 1 << 6);
	st7789_cmd(ST7789_INVON);
//	st7789_cmd(ST7789_NORON);
}
