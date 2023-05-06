/*
 * Reference code:
 * https://github.com/lupyuen/hynitron_i2c_cst0xxse/blob/master/cst0xx_core.c
 * Analysis and Rust code:
 * https://www.pcbway.com/blog/Activities/Building_a_Rust_Driver_for_PineTime_s_Touch_Controller.html
 */

#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "i2c.h"
#include "touch.h"
#include "cst816.h"


#define	TOUCH_EVENTS		10	/* maximum number of events */
#define	TOUCH_HEADER_SIZE	3
#define	TOUCH_EVENT_SIZE	6
#define	TOUCH_REG_0		0	/* first register to read */
#define	TOUCH_REGS \
	(TOUCH_HEADER_SIZE + TOUCH_MAX_EVENTS * TOUCH_EVENT_SIZE)


static unsigned touch_i2c;
static unsigned touch_addr;
static unsigned touch_int_pin;


void cst816_read(struct touch *t)
{
	uint8_t buf[TOUCH_REGS];
	unsigned n, i;

	i2c_read(touch_i2c, touch_addr, TOUCH_REG_0, buf, TOUCH_REGS);
	t->gesture = buf[1];
	n = buf[2] & 0xf;
	n = n < TOUCH_MAX_EVENTS ? n : TOUCH_MAX_EVENTS;
	t->events = n;
	for (i = 0; i != t->events; i++) {
		const uint8_t *p =
		    buf + TOUCH_HEADER_SIZE + i * TOUCH_EVENT_SIZE;
		struct touch_event *e = t->event + i;

		e->x = (p[0] << 8 | p[1]) & 0xfff;
		e->y = (p[2] << 8 | p[3]) & 0xfff;
		e->action = p[0] >> 6;
		e->finger = p[2] >> 4;
		e->pressure = p[4] >> 4;
		e->area = p[5];
	}
}


bool cst816_poll(void)
{
	return gpio_in(touch_int_pin);
}


void cst816_init(unsigned i2c, unsigned addr, unsigned int_pin)
{
	touch_i2c = i2c;
	touch_addr = addr;
	touch_int_pin = int_pin;
	gpio_cfg_in(int_pin, GPIO_PULL_UP);
}
