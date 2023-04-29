#ifndef MMIO_H
#define	MMIO_H

extern volatile void *mmio_m0_base;
extern volatile void *mmio_d0_base;


void mmio_init(void);

#endif /* !MMIO_H */
