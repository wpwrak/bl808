#ifndef GPIO_H
#define	GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include "mmio.h"


/* BL808 RM, 4.7.1 */

#define	GPIO_MASK_MODE	(3 << 30)
enum GPIO_MODE {
	GPIO_MODE_OUT		= 0,
	GPIO_MODE_SET		= 1,
	GPIO_MODE_SW_OUT	= 2,
	GPIO_MODE_SW_SET	= 3,
};
#define	GPIO_MASK_I		(1 << 28)
#define	GPIO_MASK_CLR		(1 << 26)
#define	GPIO_MASK_SET		(1 << 25)
#define	GPIO_MASK_O		(1 << 24)
#define	GPIO_MASK_INT_MASK	(1 << 22)
#define	GPIO_MASK_INT_STAT	(1 << 21)
#define	GPIO_MASK_INT_CLR	(1 << 20)
#define	GPIO_MASK_INT_MODE	(1 << 16)
enum GPIO_INT_MODE {
	GPIO_INT_MODE_FALL	= 0,	/* synchronous */
	GPIO_INT_MODE_RISE	= 1,
	GPIO_INT_MODE_LOW	= 2,
	GPIO_INT_MODE_HIGH	= 3,
	GPIO_INT_MODE_A_FALL	= 4,	/* asynchronous */
	GPIO_INT_MODE_A_RISE	= 5,
	GPIO_INT_MODE_A_LOW	= 6,
	GPIO_INT_MODE_A_HIGH	= 7,
};
#define	GPIO_MASK_FN		(0x1f << 8)
enum GPIO_FN {
	GPIO_FN_SPI_SS		= 1,
	GPIO_FN_SPI_MOSI	= 1,
	GPIO_FN_SPI_MISO	= 1,
	GPIO_FN_SPI_SCLK	= 1,
	GPIO_FN_I2S_BCLK	= 3,
	GPIO_FN_I2C0_SCL	= 5,
	GPIO_FN_I2C0_SDA	= 5,
	GPIO_FN_I2C1_SCL	= 6,
	GPIO_FN_I2C1_SDA	= 6,
	GPIO_FN_GPIO		= 11,
	GPIO_FN_I2C2_SCL	= 19,
	GPIO_FN_I2C2_SDA	= 19,
	GPIO_FN_I2C3_SCL	= 20,
	GPIO_FN_I2C3_SDA	= 20,
};
#define	GPIO_MASK_OE		(1 << 6)
#define	GPIO_MASK_PULL		(3 << 4)
enum GPIO_PULL {
	GPIO_PULL_NONE		= 0,
	GPIO_PULL_UP		= 1,
	GPIO_PULL_DOWN		= 2,
};
#define	GPIO_MASK_DRV		(3 << 2)
#define	GPIO_MASK_SMT		(1 << 1)	/* Schmitt trigger */
#define	GPIO_MASK_IE		(1 << 0)

#define	GPIO_ADD(field, value) \
	(GPIO_MASK_##field & ((value) << \
	(__builtin_ffs(GPIO_MASK_##field) - 1)))
#define	GPIO_DEL(field) (~GPIO_MASK_##field)

#define GPIO_CFG(n) \
	(*(volatile uint32_t *) ((mmio_m0_base + 0x8c4) + (4 * (n))))

/*
 * The registers with just the IO bits, without the remaining configuration
 * items are cryptically named gpio_cfg128 and such. We use better names.
 */

#define GPIO_IN0	GPIO_CFG(128)
#define GPIO_IN1	GPIO_CFG(129)
#define GPIO_OUT0	GPIO_CFG(136)
#define GPIO_OUT1	GPIO_CFG(137)
#define	GPIO_SET0	GPIO_CFG(138)
#define	GPIO_SET1	GPIO_CFG(139)
#define	GPIO_CLR0	GPIO_CFG(140)
#define	GPIO_CLR1	GPIO_CFG(141)


void gpio_cfg_off(unsigned pin);
void gpio_cfg_in(unsigned pin, enum GPIO_PULL pull);
void gpio_cfg_out(unsigned pin, bool on, int drive);
void gpio_cfg_fn(unsigned pin, enum GPIO_FN fn);


static inline bool gpio_in(unsigned pin)
{
	return GPIO_CFG(pin) & GPIO_MASK_I;
}


static inline void gpio_set(unsigned pin)
{
	if (pin < 32)
		GPIO_SET0 = 1 << pin;
	else
		GPIO_SET1 = 1 << (pin - 32);
}


static inline void gpio_clr(unsigned pin)
{
	if (pin < 32)
		GPIO_CLR0 = 1 << pin;
	else
		GPIO_CLR1 = 1 << (pin - 32);
}


static inline void gpio_out(unsigned pin, bool on)
{
	if (on)
		gpio_set(pin);
	else
		gpio_clr(pin);
}

#endif /* !GPIO_H */
