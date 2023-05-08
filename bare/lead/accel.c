#include <stdint.h>
#include <stdio.h>

#include "mmio.h"
#include "i2c.h"
#include "bma400.h"


#define	ACCEL_ADDR	0x14
#define	ACCEL_INT	31	/* IO31 */

#define	ACCEL_I2C	0
#define I2C0_SDA	7	/* O7_CAM_I2C0_SDA */
#define I2C0_SCL	6	/* IO7_CAM_I2C0_SDA */


int main(void)
{
	int id;

	mmio_init();
	i2c_init(0, I2C0_SDA, I2C0_SCL, 100);
	id = bma400_init(ACCEL_I2C, ACCEL_ADDR, ACCEL_INT);
	if (id < 0) {
		printf("bma400: initialization failed\n");
		return 1;
	}
	if (id == BMA400_CHIP_ID)
		printf("bma400: chip ID 0x%02x (BMA400)\n", id);
	else
		printf("bma400: chip ID 0x%02x (expected 0x%02x)\n",
		    id, BMA400_CHIP_ID);

	while (1) {
		struct bma400_sample s;

		while (!bma400_poll(&s));
		printf("%5d %5d %5d\n", s.x, s.y, s.z);
	}
}
