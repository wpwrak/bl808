/*
 * led.c - Blink the M1s LED at ~1 Hz
 */


#include <stdbool.h>
#include <stdint.h>

#include "mmio.h"
#include "gpio.h"


#define	LED	8	/* IO8_PWM_LED */

#define	DELAY_LOOP_HZ	119.3	/* HZ per million cycles */


int main(void)
{
	bool on = 1;
	unsigned i;

	mmio_init();
	gpio_cfg_out(LED, 0, 3);

	while (1) {
		on = !on;
		gpio_out(LED, on);
		for (i = 0; i != 1000 * 1000 * DELAY_LOOP_HZ; i++)
			asm("");
	}
}
