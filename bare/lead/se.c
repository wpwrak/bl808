/*
 * se.c - Access an ATECC508/608 Secure Element
 */

#include <stdio.h>

#include "board.h"
#include "mmio.h"
#include "i2c.h"
#include "atecc.h"


int main(void)
{
	mmio_init();
	i2c_init(0, I2C0_SDA, I2C0_SCL, 100);
	atecc_init(ATECC_I2C, ATECC_ADDR);
	atecc_info(ATECC_I2C);
	return 0;
}
