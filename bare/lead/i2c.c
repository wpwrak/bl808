#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "mmio.h"
#include "gpio.h"
#include "i2c.h"
#include "delay.h"


#define	I2C_BASE(i2c) \
	((i2c) == 0 ? mmio_m0_base + 0xa300 : \
	(i2c) == 1 ? mmio_m0_base + 0xa900 : \
	(i2c) == 2 ? mmio_d0_base + 0x3000 : mmio_d0_base + 0x4000)

#define	I2C_CONFIG(i2c) \
	(*(volatile uint32_t *) I2C_BASE(i2c))
#define	I2C_MASK_CFG_DEG_CNT		(0xf << 28)
#define	I2C_MASK_CFG_PKT_LEN		(0xff << 20)
#define	I2C_MASK_CFG_SLV_ADDR		(0x3ff << 8)
#define	I2C_MASK_CFG_10B_EN		(1 << 7)
#define	I2C_MASK_CFG_SUB_ADDR_LEN	(3 << 5)
enum I2C_SUB_ADDR_LEN {
	I2C_SUB_ADDR_LEN_1		= 0,
	I2C_SUB_ADDR_LEN_2		= 1,
	I2C_SUB_ADDR_LEN_3		= 2,
	I2C_SUB_ADDR_LEN_4		= 3,
};
#define	I2C_MASK_CFG_SUB_ADDR_EN	(1 << 4)
#define	I2C_MASK_CFG_SCL_SYNC_EN	(1 << 3)
#define	I2C_MASK_CFG_DEG_EN		(1 << 2)
#define	I2C_MASK_CFG_RnW		(1 << 1)
#define	I2C_MASK_CFG_M_EN		(1 << 0)

#define	I2C_SUB_ADDR(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x8))

#define	I2C_BUS_BUSY(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0xc))
#define I2C_MASK_BUS_BUSY_CLR		(1 << 1)
#define I2C_MASK_BUS_BUSY		(1 << 0)

#define	I2C_PRD_START(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x10))
#define	I2C_PRD_STOP(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x14))
#define	I2C_PRD_DATA(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x18))

#define I2C_CFG0(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x80))
#define	I2C_MASK_CFG0_RX_FIFO_UNDER	(1 << 7)
#define	I2C_MASK_CFG0_RX_FIFO_OVER	(1 << 6)
#define	I2C_MASK_CFG0_TX_FIFO_UNDER	(1 << 5)
#define	I2C_MASK_CFG0_TX_FIFO_OVER	(1 << 4)
#define	I2C_MASK_CFG0_RX_FIFO_CLR	(1 << 3)
#define	I2C_MASK_CFG0_TX_FIFO_CLR	(1 << 2)
#define	I2C_MASK_CFG0_RX_DMA_EN		(1 << 1)
#define	I2C_MASK_CFG0_TX_DMA_EN		(1 << 0)

#define I2C_CFG1(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x84))
#define I2C_MASK_CFG1_RX_FIFO_TH        (1 << 24)
#define I2C_MASK_CFG1_TX_FIFO_TH        (1 << 16)
#define I2C_MASK_CFG1_RX_FIFO_CNT       (3 << 8)
#define I2C_MASK_CFG1_TX_FIFO_CNT       (3 << 0)

#define I2C_WDATA(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x88))
#define I2C_RDATA(i2c) \
	(*(volatile uint32_t *) (I2C_BASE(i2c) + 0x8c))

#define	I2C_ADD(field, value) \
	(I2C_MASK_##field & ((value) << (__builtin_ffs(I2C_MASK_##field) - 1)))
#define I2C_DEL(field) (~I2C_MASK_##field)
#define I2C_GET(field, reg) \
	(((I2C_MASK_##field) & (reg)) >> (__builtin_ffs(I2C_MASK_##field) - 1))

#define	XCLK_kHz	40000	/* 40 MHz XTAL clock */


static void i2c_setup(unsigned i2c, unsigned addr, unsigned reg, bool rd,
    unsigned len)
{
	uint32_t cfg;

	assert(len);
	I2C_SUB_ADDR(i2c) = reg;
	cfg = I2C_CONFIG(i2c);
	cfg &= I2C_DEL(CFG_PKT_LEN) & I2C_DEL(CFG_SLV_ADDR) &
	    I2C_DEL(CFG_SUB_ADDR_LEN) & I2C_DEL(CFG_10B_EN) &
	    I2C_DEL(CFG_M_EN) & I2C_DEL(CFG_RnW);
	cfg |= I2C_ADD(CFG_PKT_LEN, len - 1) | I2C_ADD(CFG_SLV_ADDR, addr) |
	    I2C_ADD(CFG_SUB_ADDR_LEN, I2C_SUB_ADDR_LEN_1) |
	    I2C_ADD(CFG_SUB_ADDR_EN, 1) | I2C_ADD(CFG_SCL_SYNC_EN, 1) |
	    I2C_ADD(CFG_DEG_EN, 1) | I2C_ADD(CFG_RnW, rd);
	I2C_CONFIG(i2c) = cfg;
//printf("CFG(0) 0x%08x\n", cfg);
	cfg |= I2C_ADD(CFG_M_EN, 1);
	I2C_CONFIG(i2c) = cfg;
//printf("CFG(1) 0x%08x\n", cfg);
}


static void i2c_end(unsigned i2c)
{
	I2C_CONFIG(i2c) &= I2C_DEL(CFG_M_EN);
}


unsigned i2c_write(unsigned i2c, unsigned addr, unsigned reg,
    const void *data, unsigned len)
{
	const uint8_t *d = data;
	unsigned left = 0;

	i2c_setup(i2c, addr, reg, 0, len);
	while (1) {
		uint32_t word = d[0];

		switch (left) {
		default:
			word |= d[3] << 24;
			/* fall through */
		case 3:
			word |= d[2] << 16;
			/* fall through */
		case 2:
			word |= d[1] << 8;
			/* fall through */
		case 1:
			break;
		}
		while (!I2C_GET(CFG1_TX_FIFO_CNT, I2C_CFG1(0)));
		I2C_WDATA(i2c) = word;
		if (left <= 4)
			break;
		left -= 4;
		d += 4;
	}
	i2c_end(i2c);
	return len;
}


bool i2c_read(unsigned i2c, unsigned addr, unsigned reg,
    void *data, unsigned len)
{
	uint8_t *d = data;

//printf("F0: 0x%08x\n", I2C_CFG1(0));
	i2c_setup(i2c, addr, reg, 1, len);
//mdelay(1);
	while (1) {
		uint32_t word;

//printf("F1: 0x%08x\n", I2C_CFG1(0));
		while (!I2C_GET(CFG1_RX_FIFO_CNT, I2C_CFG1(0)));
		word = I2C_RDATA(i2c);
//printf("D: 0x%08x\n", word);

		switch (len) {
		default:
			d[3] = word >> 24;
			/* fall through */
		case 3:
			d[2] = word >> 16;
			/* fall through */
		case 2:
			d[1] = word >> 8;
			/* fall through */
		case 1:
			d[0] = word;
			break;
		}
		if (len <= 4)
			break;
		len -= 4;
		d += 4;
	}
	i2c_end(i2c);
	return 1;
}


void i2c_init(unsigned i2c, unsigned sda, unsigned scl, unsigned kHz)
{
	/* SDA and SCL share the same GPIO_FN codepoint */
	static const uint8_t fn[] = { GPIO_FN_I2C0_SDA, GPIO_FN_I2C1_SDA,
	    GPIO_FN_I2C2_SDA, GPIO_FN_I2C3_SDA };
	unsigned phase;

	/*
	 * @@@ copy fancy adjustments from
	 * bouffalo_sdk/drivers/lhal/src/bflb_i2c.c:bflb_i2c_set_frequence
	 * ?
	 */
	phase = (XCLK_kHz / kHz / 4) - 1;
	phase = phase > 255 ? 255 : phase ? phase : 1;
//printf("phase %u\n", phase);
	I2C_PRD_START(i2c) = I2C_PRD_STOP(i2c) = I2C_PRD_DATA(i2c) =
	    0x1010101 * phase;

	gpio_cfg_out(sda, 1, 3);
	gpio_cfg_out(scl, 1, 3);
	gpio_cfg_in(sda, GPIO_PULL_UP);
	gpio_cfg_in(scl, GPIO_PULL_UP);
	gpio_cfg_fn(sda, fn[i2c]);
	gpio_cfg_fn(scl, fn[i2c]);

	/* clean up in case we got interrupted before */

	i2c_end(i2c);
	I2C_BUS_BUSY(i2c) = I2C_MASK_BUS_BUSY_CLR;
	mdelay(1);

	I2C_CFG0(i2c) = I2C_ADD(CFG0_RX_FIFO_CLR, 1) |
	    I2C_ADD(CFG0_TX_FIFO_CLR, 1);
}
