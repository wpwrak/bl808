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
	uint32_t value;
	bool on = 0;
	unsigned i;

	mmio_init();
	value = GPIO_CFG(LED);
	value &= GPIO_DEL(MODE) | GPIO_DEL(FN) | GPIO_DEL(OE);
	value |= GPIO_ADD(MODE, GPIO_MODE_OUT) |
	    GPIO_ADD(FN, 0) | GPIO_ADD(OE, 1);
	GPIO_CFG(LED) = value;

	while (1) {
		on = !on;
		value = (value & GPIO_DEL(O)) | GPIO_ADD(O, on);
		GPIO_CFG(LED) = value;
		for (i = 0; i != 1000 * 1000 * DELAY_LOOP_HZ; i++)
			asm("");
	}
}
