#ifndef TOUCH_H
#define	TOUCH_H

#include <stdint.h>


#define TOUCH_MAX_EVENTS	10      /* maximum number of events */


enum touch_gesture {
	touch_g_none	= 0,	/* no gesture */
	touch_g_down	= 1,	/* slide */
	touch_g_up	= 2,
	touch_g_left	= 3,
	touch_g_right	= 4,
	touch_g_single	= 5,	/* click */
	touch_g_double	= 0xb,
	touch_g_long	= 0xc,
	
};

enum touch_action {
	touch_down	= 0,
	touch_up	= 1,
	touch_contact	= 2,
};


struct touch {
	uint8_t	gesture;
	uint8_t events;
	struct touch_event {
		enum touch_action action;
		uint16_t x, y;
		uint8_t finger;
		uint8_t pressure;
		uint8_t area;
	} event[TOUCH_MAX_EVENTS];
};

#endif /* !TOUCH_H */
