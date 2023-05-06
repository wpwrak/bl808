#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "mmio.h"


/*
 * DMA_C7RSVD from section 7.5.62
 * DMA0 and DMA1 from table 1.3 (Memory Map) in section 1.4
 */

#define	MMIO_M0_BASE	(0x20000000)
#define	MMIO_M0_SIZE	(0x71900)	/* DMA_C7RSVD - DMA0 + DMA1 */

#define	MMIO_D0_BASE	(0x30000000)
#define	MMIO_D0_SIZE	(0x9fff)	/* 0xfff - DMA2 + TIMER1 */


volatile void *mmio_m0_base;
volatile void *mmio_d0_base;


void mmio_init(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	mmio_m0_base = mmap(NULL, MMIO_M0_SIZE, PROT_READ | PROT_WRITE,
	    MAP_SHARED, fd, MMIO_M0_BASE);
	if (mmio_m0_base == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	mmio_d0_base = mmap(NULL, MMIO_D0_SIZE, PROT_READ | PROT_WRITE,
	    MAP_SHARED, fd, MMIO_D0_BASE);
	if (mmio_d0_base == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
}
