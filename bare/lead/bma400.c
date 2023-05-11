#include <stdbool.h>
#include <stdint.h>

#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "bma400.h"


#define	BMA400_CHIPID		0x00
#define	BMA400_STATUS		0x03
#define	BMA400_STATUS_DRDY_STAT			0x80
#define	BMA400_STATUS_CMD_RDY			0x10
#define	BMA400_STATUS_POWER_MODE_MASK		0x06
enum BMA400_POWER_MODE {
	BMA400_POWER_MODE_SLEEP		= 0,
	BMA400_POWER_MODE_LOW		= 1,
	BMA400_POWER_MODE_NORMAL	= 2,
};
#define	BMA400_STATUS_INT_ACTIVE	0x01

#define	BMA400_ACC	0x04	/* X, Y, Z; LSB then  MSB */

#define	BMA400_ACC_CONFIG0		0x19
#define	BMA400_ACC_CONFIG_POWER_MODE_MASK	0x03
/* enum BMA400_POWER_MODE */
#define	BMA400_ACC_CONFIG_OSR_LP		0x40	/* 0.2 or 0.4 x ODR */
#define	BMA400_ACC_CONFIG_FILT1_BW		0x80

#define	BMA400_ACC_CONFIG1		0x1a
#define	BMA400_ACC_CONFIG_ACC_ODR_MASK	0x0f
enum BMA400_ODR {
	BMA400_ODR_12	= 0x05,	/* 12.5 Hz */
	BLA400_ODR_25	= 0x06,	/* 25 Hz */
	BLA400_ODR_50	= 0x07,	/* 50 Hz */
	BLA400_ODR_100	= 0x08,	/* 100 Hz */
	BLA400_ODR_200	= 0x09,	/* 200 Hz */
	BLA400_ODR_400	= 0x0a,	/* 400 Hz */
	BLA400_ODR_800	= 0x0b,	/* 800 Hz */
};
#define	BMA400_ACC_CONFIG_ACC_OSR_MASK	0x30
enum BMA400_OSR {
	BMA400_OSR_LOWEST	= 0,
	BMA400_OSR_LOW		= 1,
	BMA400_OSR_HIGH		= 2,
	BMA400_OSR_HIGHEST	= 3,
};
#define	BMA400_ACC_CONFIG_ACC_RANGE_MASK 0xc0
enum BMA400_RANGE {
	BMA400_RANGE_2G		= 0,
	BMA400_RANGE_4G		= 1,
	BMA400_RANGE_8G		= 2,
	BMA400_RANGE_16G	= 3,
};

#define	BMA400_ACC_CONFIG2		0x1b
#define	BMA400_ACC_CONFIG_SRC_MASK	0x0c
enum BMA400_SRC {
	BMA400_SRC_FILT1		= 0,	// or 3
	BMA400_SRC_FILT2		= 1,
	BMA400_SRC_FILT_LP		= 2,
};

#define	BMA400_MASK(field)	BMA400_##field##_MASK
#define	BMA400_SHIFT(field)	(__builtin_ffs(BMA400_##field##_MASK) - 1)


static unsigned bma400_i2c;
static unsigned bma400_addr;
static unsigned bma400_int_pin;


#include <stdlib.h>
#include <stdio.h>


static void bma400_write_reg(uint8_t reg, uint8_t value)
{
//printf("0x%02x = 0x%02x\n", reg, value);
	i2c_write(bma400_i2c, bma400_addr, reg, &value, 1);
}


bool bma400_poll(struct bma400_sample *s)
{
	uint8_t status;

	i2c_read(bma400_i2c, bma400_addr, BMA400_STATUS, &status, 1);
	if (!(status & BMA400_STATUS_DRDY_STAT))
		return 0;
	i2c_read(bma400_i2c, bma400_addr, BMA400_ACC, s, 3 * 2);
	/* sign-extend */
	if (s->x & 0x800)
		s->x |= 0xf000;
	if (s->y & 0x800)
		s->y |= 0xf000;
	if (s->z & 0x800)
		s->z |= 0xf000;
	return 1;
}


int bma400_init(unsigned i2c, unsigned addr, unsigned int_pin)
{
	uint8_t id;

	bma400_i2c = i2c;
	bma400_addr = addr;
	bma400_int_pin = int_pin;
	gpio_cfg_in(int_pin, GPIO_PULL_UP);

	i2c_read(bma400_i2c, bma400_addr, BMA400_CHIPID, &id, 1);

	/* @@@ soft-reset ? */

	/* wake up */
	bma400_write_reg(BMA400_ACC_CONFIG0,
	    BMA400_POWER_MODE_NORMAL << BMA400_SHIFT(ACC_CONFIG_POWER_MODE));
	mdelay(2);	/* page 13, 1500 us */

	bma400_write_reg(BMA400_ACC_CONFIG1,
	    0x38);
	bma400_write_reg(BMA400_ACC_CONFIG2,
	    BMA400_SRC_FILT2 << BMA400_SHIFT(ACC_CONFIG_SRC));

	return id;
}
