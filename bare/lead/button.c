/*
 * button.c - Read M1s buttons
 */

#include <stdbool.h>
#include <stdio.h>

#include "board.h"
#include "delay.h"
#include "mmio.h"
#include "gpio.h"


int main(void)
{
	int last_l = -1, last_r = -1;

	mmio_init();
	gpio_cfg_in(BUTTON_L, GPIO_PULL_UP);
	gpio_cfg_in(BUTTON_R, GPIO_PULL_UP);
	while (1) {
		bool changed = 0;
		bool in;

		in = gpio_in(BUTTON_L);
		if (last_l != in) {
			changed = 1;
			last_l = in;
		}
		in = gpio_in(BUTTON_R);
		if (last_r != in) {
			changed = 1;
			last_r = in;
		}
		if (changed)
			printf("%c%c\n",
			    last_l ? '-' : 'L', last_r ? '-' : 'R');
		mdelay(10);
	}
}
