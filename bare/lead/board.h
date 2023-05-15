/*
 * board.h - Board definitions
 */

/*
 * @@@ This is for an M1s Dock with the following additional devices:
 * - Display module (TFT controller on SPI, touch panel controller on I2C)
 * - BMA400 acceleration sensor on I2C
 * - ATECC608 Secure Element on I2C
 */

#ifndef BOARD_H
#define	BOARD_H

/* --- M1s I2C ------------------------------------------------------------- */

#define I2C0_SDA	7	/* IO7_CAM_I2C0_SDA */
#define I2C0_SCL	6	/* IO7_CAM_I2C0_SDA */

/* --- LED ----------------------------------------------------------------- */

#define LED		8	/* IO8_PWM_LED */

/* --- Display module: TFT ------------------------------------------------- */

#define LCD_CS		12	/* IO12_LCD_DBI_CS */
#define LCD_DnC		13	/* IO13_LCD_DBI_DC */
#define LCD_MOSI	25	/* IO25_LCD_DBI_SDA */
#define LCD_SCLK	19	/* IO19_LCD_DBI_SCK */

#define LCD_RST		24	/* IO24_LCD_RESET */

#define LCD_SPI		0

#define LCD_WIDTH	240
#define LCD_HEIGHT	280

/* --- Display module: backlight ------------------------------------------- */

#define LCD_BL		11	/* IO11_LCD_BL_PWM */

/* --- Display module: touch screen ---------------------------------------- */

#define TOUCH_INT	32	/* IO32_TP_TINT */

#define TOUCH_I2C	0
#define TOUCH_I2C_ADDR	0x15

/* --- BMA400 -------------------------------------------------------------- */

#define ACCEL_INT	31	/* IO31 */

#define ACCEL_I2C	0
#define ACCEL_ADDR	0x14

/* --- ATECC608 ------------------------------------------------------------ */

#define ATECC_I2C	0
#define ATECC_ADDR	0x60

#endif /* !BOARD_H */
