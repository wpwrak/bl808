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
#include "cst816.h"


#define	CST816_EVENTS		10	/* maximum number of events */
#define	CST816_HEADER_SIZE	3
#define	CST816_EVENT_SIZE	6
#define	CST816_REG_0		0	/* first register to read */
#define	CST816_REGS \
	(CST816_HEADER_SIZE + CST816_MAX_EVENTS * CST816_EVENT_SIZE)


static unsigned cst816_i2c;
static unsigned cst816_addr;
static unsigned cst816_int_pin;


void cst816_read(struct cst816_touch *t)
{
	uint8_t buf[CST816_REGS];
	unsigned n, i;

	i2c_read(cst816_i2c, cst816_addr, CST816_REG_0, buf, CST816_REGS);
	t->gesture = buf[1];
	n = buf[2] & 0xf;
	n = n < CST816_MAX_EVENTS ? n : CST816_MAX_EVENTS;
	t->events = n;
	for (i = 0; i != t->events; i++) {
		const uint8_t *p =
		    buf + CST816_HEADER_SIZE + i * CST816_EVENT_SIZE;
		struct cst816_event *e = t->event + i;

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
	return gpio_in(cst816_int_pin);
}


void cst816_init(unsigned i2c, unsigned addr, unsigned int_pin)
{
	cst816_i2c = i2c;
	cst816_addr = addr;
	cst816_int_pin = int_pin;
	gpio_cfg_in(int_pin, GPIO_PULL_UP);
}
