/*
 * clk.h - Clocks and sundry global registers
 */

#ifndef CLK_H
#define	CLK_H

#include <stdint.h>

#include "mmio.h"


/* bl_mcu_sdk/drivers/soc/bl602/std/include/hardware/glb_reg.h */


#define	CLK_SPI0_CFG \
	(*(volatile uint32_t *) (mmio_m0_base + 0x1b0))
#define	CLK_SPI_MASK_SWAP	(0xf << 16)
#define	CLK_SPI_MASK_SEL	(1 << 9)
#define	CLK_SPI_MASK_EN		(1 << 8)
#define	CLK_SPI_MASK_DIV	(0x1f << 0)

#define	CLK_SDH_CFG \
	(*(volatile uint32_t *) (mmio_m0_base + 0x430))
#define	CLK_SDH_MASK_EN		(1 << 13)
#define	CLK_SDH_MASK_SEL	(1 << 12)
#define	CLK_SDH_MASK_DIV	(7 << 9)

#define	CLK_I2C_CFG \
	(*(volatile uint32_t *) (mmio_m0_base + 0x180))
#define	CLK_I2C_MASK_SEL	(1 << 25)
#define	CLK_I2C_MASK_EN		(1 << 24)
#define	CLK_I2C_MASK_DIV	(0xff << 16)

#define	GLB_PARM \
	(*(volatile uint32_t *) (mmio_m0_base + 0x510))
#define	GLB_PARM_MASK_SPI0_MASTER	(1 << 12)

#endif /* !CLK_H */
