/*
 * delay.h - Delay loops
 */

#ifndef DELAY_H
#define	DELAY_H

#define DELAY_LOOP_HZ	119.3	/* HZ per million cycles */


static inline void mdelay(unsigned ms)
{
	unsigned i;

	while (ms--)
		for (i = 0; i != 1000 * DELAY_LOOP_HZ; i++)
			asm("");
}

#endif /* !DELAY_H */
