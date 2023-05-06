#ifndef CST816_H
#define	CST816_H

#include <stdbool.h>
#include <stdint.h>


#define CST816_MAX_EVENTS	10      /* maximum number of events */


enum cst816_gesture {
	cst816_g_none	= 0,	/* no gesture */
	cst816_g_down	= 1,	/* slide */
	cst816_g_up	= 2,
	cst816_g_left	= 3,
	cst816_g_right	= 4,
	cst816_g_single	= 5,	/* click */
	cst816_g_double	= 0xb,
	cst816_g_long	= 0xc,
	
};

enum cst816_action {
	cst816_down	= 0,
	cst816_up	= 1,
	cst816_contact	= 2,
};


struct cst816_touch {
	enum cst816_gesture gesture;
	uint8_t events;
	struct cst816_event {
		enum cst816_action action;
		uint16_t x, y;
		uint8_t finger;
		uint8_t pressure;
		uint8_t area;
	} event[CST816_MAX_EVENTS];
};


void cst816_read(struct cst816_touch *t);
bool cst816_poll(void);
void cst816_init(unsigned i2c, unsigned addr, unsigned int_pin);

#endif /* !CST816_H */
