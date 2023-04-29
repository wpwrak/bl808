/*
 * lcd.c - LCD driver (ST7789V on M1s)
 */


#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "spi.h"
#include "delay.h"
#include "lcd.h"


/* --- M1s LCD connections ------------------------------------------------- */

#define	LCD_CS		12	/* IO12_LCD_DBI_CS */
#define	LCD_DnC		13	/* IO13_LCD_DBI_DC */
#define	LCD_MOSI	25	/* IO25_LCD_DBI_SDA */
#define	LCD_SCLK	19	/* IO19_LCD_DBI_SCK */

#define	LCD_RST		24	/* IO24_LCD_RESET */

#define	LCD_BL		11	/* IO11_LCD_BL_PWM */


/* --- ST7789V commands ---------------------------------------------------- */

#define	LCD_SLPOUT	0x11
#define	LCD_NORON	0x13
#define	LCD_INVON	0x21
#define	LCD_DISPON	0x29
#define	LCD_CASET	0x2a
#define	LCD_RASET	0x2b
#define	LCD_RAMWR	0x2c
#define	LCD_MADCTL	0x36
#define	LCD_COLMOD	0x3a


/* --- Send commands and parameters ---------------------------------------- */


static void lcd_cmd(uint8_t cmd)
{
	spi_start();
	gpio_out(LCD_DnC, 0);
	spi_send(&cmd, 1);
	spi_end();
}


static void lcd_cmd8(uint8_t cmd, uint8_t value)
{
	spi_start();
	gpio_out(LCD_DnC, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(LCD_DnC, 1);
	spi_send(&value, 1);
	spi_end();
}


static void lcd_cmd32(uint8_t cmd, uint32_t value)
{
	uint8_t tmp[] = { value >> 24, value >> 16, value >> 8, value };

	spi_start();
	gpio_out(LCD_DnC, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(LCD_DnC, 1);
	spi_send(tmp, 4);
	spi_end();
}


static void lcd_send(uint8_t cmd, const void *buf, unsigned len)
{
	spi_start();
	gpio_out(LCD_DnC, 0);
	spi_send(&cmd, 1);
	spi_sync();
	gpio_out(LCD_DnC, 1);
	spi_send(buf, len);
	spi_end();
}


/* --- API ----------------------------------------------------------------- */


void lcd_update(const void *fb, unsigned x0, unsigned y0, unsigned x1,
    unsigned y1)
{
	lcd_cmd32(LCD_CASET, x0 << 16 | x1);
	/* @@@ no idea why we need this offset, but we do ... */
	lcd_cmd32(LCD_RASET, (y0 + 20) << 16 | (y1 + 20));

	lcd_send(LCD_RAMWR, fb, (x1 - x0 + 1) * (y1 - y0 + 1) * 2);

	/* @@@ we need a small delay between frames */
	mdelay(1);
}


void lcd_on(void)
{
	lcd_cmd(LCD_DISPON);
	mdelay(1);
	gpio_out(LCD_BL, 1);
}


void lcd_init(void)
{
	gpio_cfg_out(LCD_RST, 1, 0);
	gpio_cfg_out(LCD_DnC, 1, 0);
	gpio_cfg_out(LCD_BL, 0, 0);

	/*
	 * ST7789V can go as fast as 1 / 66 ns = 15.15 MHz
	 */
	spi_init(LCD_MOSI, LCD_SCLK, LCD_CS, 15);

	/* --- reset and wakeup --- */

	gpio_out(LCD_RST, 0);
	mdelay(1);
	gpio_out(LCD_RST, 1);
	mdelay(120);

	lcd_cmd(LCD_SLPOUT);			// exit sleep mode
        mdelay(120);				// SLPOUT-SLPIN timing

	/* --- display organization --- */

	lcd_cmd8(LCD_COLMOD, 5 << 4 | 5);	// pixel format
	lcd_cmd8(LCD_MADCTL, 1 << 7 | 1 << 6);
	lcd_cmd(LCD_INVON);
//	lcd_cmd(LCD_NORON);
}
