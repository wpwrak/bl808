#include <stdbool.h>

#include "gpio.h"


void gpio_cfg_off(unsigned pin)
{
	uint32_t cfg;

	cfg = GPIO_CFG(pin);
	cfg &= GPIO_DEL(FN) & GPIO_DEL(OE) & GPIO_DEL(IE) & GPIO_DEL(PULL);
	GPIO_CFG(pin) = cfg;
}


void gpio_cfg_in(unsigned pin, enum GPIO_PULL pull)
{
	uint32_t cfg;

	cfg = GPIO_CFG(pin);
	cfg &= GPIO_DEL(FN) & GPIO_DEL(OE) & GPIO_DEL(PULL);
	cfg |= GPIO_ADD(FN, GPIO_FN_GPIO) | GPIO_ADD(IE, 1) |
	    GPIO_ADD(PULL, pull);
	GPIO_CFG(pin) = cfg;
}


void gpio_cfg_out(unsigned pin, bool on, int drive)
{
	uint32_t cfg;

	cfg = GPIO_CFG(pin);
	cfg &= GPIO_DEL(MODE) & GPIO_DEL(FN) & GPIO_DEL(IE) & GPIO_DEL(O) &
	    GPIO_DEL(PULL) & GPIO_DEL(DRV);
	cfg |= GPIO_ADD(MODE, GPIO_MODE_SET) | GPIO_ADD(OE, 1) |
	    GPIO_ADD(O, on) | GPIO_ADD(FN, GPIO_FN_GPIO) |
	    GPIO_ADD(DRV, drive & 3);
	GPIO_CFG(pin) = cfg;
}


void gpio_cfg_fn(unsigned pin, enum GPIO_FN fn)
{
	uint32_t cfg;

	cfg = GPIO_CFG(pin);
	cfg &= GPIO_DEL(FN);
	cfg |= GPIO_ADD(FN, fn);
	GPIO_CFG(pin) = cfg;
}
