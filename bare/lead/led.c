/*
 * led.c - Blink the M1s LED at ~1 Hz
 */


#include <stdbool.h>
#include <stdint.h>

#include "board.h"
#include "delay.h"
#include "mmio.h"
#include "gpio.h"


int main(void)
{
	bool on = 1;

	mmio_init();
	gpio_cfg_out(LED, 0, 3);

	while (1) {
		on = !on;
		gpio_out(LED, on);
		mdelay(500);
	}
}
