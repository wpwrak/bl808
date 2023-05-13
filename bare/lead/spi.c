/*
 * spi.c - TX-only SPI0 driver
 */

/*
 * Things that should be improved:
 *
 * - also support SPI1
 * - support RX
 * - better end-of-transmission synchronization
 * - use DMA
 */

#include <stdint.h>

#include "mmio.h"
#include "gpio.h"
#include "clk.h"
#include "spi.h"


#define	SPI_BASE(spi) \
	((spi) ? mmio_d0_base + 0x8000 : mmio_m0_base + 0xa200)

#define	SPI_CFG(spi) \
	(*(volatile uint32_t *) SPI_BASE(spi))
#define	SPI_MASK_CFG_DEG_CNT		(0xf << 12)
#define	SPI_MASK_CFG_DEG_EN		(1 << 11)
#define	SPI_MASK_CFG_S_3PIN		(1 << 10)	/* S: doesn't use SS */
#define	SPI_MASK_CFG_M_CONT		(1 << 9)	/* M: : continuous */
#define	SPI_MASK_CFG_RX_IGN		(1 << 8)
#define	SPI_MASK_CFG_BYTE_BE		(1 << 7)	/* big-endian bytes */
#define	SPI_MASK_CFG_BIT_LE		(1 << 6)	/* little-endian bits */
#define	SPI_MASK_CFG_SCLK_PH		(1 << 5)
#define	SPI_MASK_CFG_SCLK_POL		(1 << 4)
#define SPI_MASK_CFG_FRAME_SIZE		(3 << 2)
enum SPI_FRAME_SIZE {
	SPI_FRAME_SIZE_8	= 0,
	SPI_FRAME_SIZE_16	= 1,
	SPI_FRAME_SIZE_24	= 2,
	SPI_FRAME_SIZE_32	= 3,
};
#define	SPI_MASK_CFG_S_EN		(1 << 1)	/* slave enable */
#define	SPI_MASK_CFG_M_EN		(1 << 0)	/* master enable */

#define	SPI_INT_STS(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x4))
#define	SPI_BUS_BUSY(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x8))
#define	SPI_PRD0(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x10))
#define	SPI_PRD1(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x14))

#define	SPI_CFG0(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x80))
#define	SPI_MASK_CFG0_RX_UNDER		(1 << 7)
#define	SPI_MASK_CFG0_RX_OVER		(1 << 6)
#define	SPI_MASK_CFG0_TX_UNDER		(1 << 5)
#define	SPI_MASK_CFG0_TX_OVER		(1 << 4)
#define	SPI_MASK_CFG0_RX_CLR		(1 << 3)
#define	SPI_MASK_CFG0_TX_CLR		(1 << 2)
#define	SPI_MASK_CFG0_RX_DMA_EN	(1 << 1)
#define	SPI_MASK_CFG0_TX_DMA_EN	(1 << 0)

#define	SPI_CFG1(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x84))
#define SPI_MASK_CFG1_RX_FIFO_TH	(0x1f << 24)
#define SPI_MASK_CFG1_TX_FIFO_TH	(0x1f << 16)
#define	SPI_MASK_CFG1_RX_FIFO_CNT	(0x3f << 8)
#define	SPI_MASK_CFG1_TX_FIFO_CNT	(0x3f << 0)

#define	SPI_WDATA(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x88))
#define	SPI_RDATA(spi) \
	(*(volatile uint32_t *) (SPI_BASE(spi) + 0x8c))

#define	SPI_ADD(field, value) \
	(SPI_MASK_##field & ((value) << (__builtin_ffs(SPI_MASK_##field) - 1)))
#define	SPI_DEL(field) (~SPI_MASK_##field)
#define	SPI_GET(field, reg) \
	(((SPI_MASK_##field) & (reg)) >> (__builtin_ffs(SPI_MASK_##field) - 1))


static unsigned spi_ss;


void spi_sync(void)
{
	unsigned i;

	/*
	 * @@@ about 500 us
	 * If we reduce the delay to ~50 us, the LCD doesn't work.
	 * This probably isn't an issue of SPI but of the LCD.
	 * Maybe st7789.c needs some more delays ?
	 */
	for (i = 0; i != 1000; i++)
		asm("");
}


void spi_start(void)
{
	gpio_out(spi_ss, 0);
}


void spi_end(void)
{
	spi_sync();
	gpio_out(spi_ss, 1);
	spi_sync();
}


void spi_send(const void *data, unsigned len)
{
	while (len--) {
		while (!SPI_GET(CFG1_TX_FIFO_CNT, SPI_CFG1(0)));
		SPI_WDATA(0) = *(uint8_t *) data++;
	}
}


void spi_init(unsigned mosi, unsigned sclk, unsigned ss, unsigned MHz)
{
	SPI_CFG(0) = SPI_MASK_CFG_SCLK_PH | SPI_MASK_CFG_M_EN;
        GLB_PARM |= GLB_PARM_MASK_SPI0_MASTER; /* important !!! */

        /*
	 * Defaults to SCLK = 160 MHz / (15 + 1) / 2
	 *  15 = value in register
	 * + 1 = counts to zero
	 * / 2 = high and low phase
	 * 160 MHz / (5 + 1) / 2 = 13.333 MHz
	 */
	SPI_PRD0(0) = 0x1010101 * (160 / 2 / MHz);

	spi_ss = ss;
	gpio_cfg_out(ss, 1, 0);
	gpio_cfg_out(sclk, 0, 0);
	gpio_cfg_out(mosi, 0, 0);

	gpio_cfg_fn(mosi, GPIO_FN_SPI_MOSI);
	gpio_cfg_fn(sclk, GPIO_FN_SPI_SCLK);
}
