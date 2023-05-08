#ifndef BMA400_H
#define	BMA400_H

#include <stdbool.h>
#include <stdint.h>


#define	BMA400_CHIP_ID	0x90


struct bma400_sample {
	int16_t x, y, z;
} __attribute__((packed));


bool bma400_poll(struct bma400_sample *s);
int bma400_init(unsigned i2c, unsigned addr, unsigned int_pin);

#endif /* !BMA400_H */
