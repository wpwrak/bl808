/*
 * se.c - Access an ATECC508/608 Secure Element
 */

#include <stdio.h>

#include "mmio.h"
#include "i2c.h"
#include "atecc.h"


#define	ATECC_ADDR	0x60

#define	ATECC_I2C	0
#define I2C0_SDA	7	/* IO7_CAM_I2C0_SDA */
#define I2C0_SCL	6	/* IO7_CAM_I2C0_SDA */


int main(void)
{
	mmio_init();
	i2c_init(0, I2C0_SDA, I2C0_SCL, 100);
	atecc_init(ATECC_I2C, ATECC_ADDR);
	atecc_info(ATECC_I2C);
	return 0;
}
