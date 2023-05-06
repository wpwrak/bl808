#include <stdbool.h>
#include <stdio.h>

#include "mmio.h"
#include "i2c.h"
#include "cst816.h"
#include "touch.h"


#define TOUCH_I2C_ADDR	0x15	/* first register to read */
#define TOUCH_INT	32	/* IO32_TP_TINT */

#define TOUCH_I2C	0
#define I2C0_SDA	7	/* O7_CAM_I2C0_SDA */
#define I2C0_SCL	6	/* IO7_CAM_I2C0_SDA */


static void decode_event(const struct touch_event *e)
{
	printf("\t");
	switch (e->action) {
	case touch_down:
		printf("DOWN");
		break;
	case touch_up:
		printf("UP");
		break;
	case touch_contact:
		printf("CONTACT");
		break;
	default:
		printf("0x%x", e->action);
	}
	printf(" X%u Y%u F%u P%u A%u\n",
	    e->x, e->y, e->finger, e->pressure, e->area);
}


static void decode_touch(const struct touch *t)
{
	unsigned i;

	switch (t->gesture) {
	case touch_g_none:
		printf("NONE");
		break;
	case touch_g_down:
		printf("SLIDE DOWN");
		break;
	case touch_g_up:
		printf("SLIDE UP");
		break;
	case touch_g_left:
		printf("SLIDE LEFT");
		break;
	case touch_g_right:
		printf("SLIDE RIGHT");
		break;
	case touch_g_single:
		printf("SINGLE");
		break;
	case touch_g_double:
		printf("DOUBLE");
		break;
	case touch_g_long:
		printf("LONG");
		break;
	default:
		printf("0x%x", t->gesture);
	}
	printf(" %u:\n", t->events);
	for (i = 0; i != t->events; i++)
		decode_event(t->event + i);
}


int main(void)
{
	int last = -1;
	bool on;

	mmio_init();
	i2c_init(0, I2C0_SDA, I2C0_SCL, 100);
	cst816_init(TOUCH_I2C, TOUCH_I2C_ADDR, TOUCH_INT);
	while (1) {
		struct touch t;

		on = !cst816_poll();
		if (on == last)
			continue;
//		printf("%u\n", on);
		if (on) {
			cst816_read(&t);
			decode_touch(&t);
//			printf("0x%x %u:0x%x 0x%x\n",
//			    t.gesture, t.events, t.event[0].x, t.event[0].y);
		}
		last = on;
	}
}
