/*
 * crc16.c - Calculate CRC-16 with polynom x^16 + x^15 + x^2 + x^1
 */

/*
 * Based on
 * https://stackoverflow.com/a/10569892/8179289
 */

#include <stdbool.h>
#include <stdint.h>

#include "crc16.h"


#define	POLY	0x8005


void crc16_begin(uint16_t *crc, uint16_t initial)
{
	*crc = initial;
}


static void crc16_add_byte(uint16_t *crc, uint8_t v)
{
	uint8_t bit;

	for (bit = 0; bit != 8; bit++) {
		bool flag = *crc & 0x8000;

		*crc <<= 1;
		*crc |= v & 1;
		v >>= 1;
		if (flag)
			*crc ^= POLY;
	}
}


uint16_t crc16_finish(uint16_t *crc)
{
	crc16_add_byte(crc, 0);
	crc16_add_byte(crc, 0);
	return *crc;
}


void crc16_add(uint16_t *crc, const void *data, unsigned len)
{
	while (len--)
		crc16_add_byte(crc, *(const uint8_t *) data++);
}


#ifdef TEST

#include <stdio.h>


int main(void)
{
	uint16_t crc;

	crc16_begin(&crc, 0);
	crc16_add(&crc, "123456789", 9);
	printf("0x%04x\n", crc16_finish(&crc));
	return 0;
}

#endif /* TEST */
