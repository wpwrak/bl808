/*
 * atecc.c - Driver for ATECC508A and similar
 */

/*
 * @@@ This is just an extremely crude proof of concept so far.
 * Known issues:
 * - bytes read from the ATECC's output FIFO are not consumed when read
 * - the BL808 I2C IP seems to be unable to send a message with content 0xc0
 *   0x00 (address 0x60, payload 0x00)
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "delay.h"
#include "i2c.h"
#include "crc16.h"
#include "atecc.h"


#define	DEBUG	0	/* 0 or non-zero */


enum atecc_word_addr {
	ATECC_RESET	= 0,
	ATECC_SLEEP	= 1,
	ATECC_IDLE	= 2,
	ATECC_COMMAND	= 3,
};

enum atecc_commad {
	ATECC_INFO	= 0x30,
};

enum atecc_info {
	ATECC_INFO_REVISION	= 0,
};


static const unsigned atecc_exec_ms[] = {
	[ATECC_INFO]		= 1,
};

static unsigned atecc_i2c;
static unsigned atecc_addr;


/* --- I2C transfers ------------------------------------------------------- */


static void dump(const char *s, const void *data, unsigned len)
{
#if DEBUG
	printf("%s:", s);
	while (len--)
		printf(" %02x", *(const uint8_t *) data++);
	printf("\n");
#endif
}


bool atecc_command(uint8_t cmd, uint8_t param1, uint16_t param2,
    const void *data, unsigned data_len)
{
	/* length, command, param1, param2, data, crc */
	uint8_t len = 1 + 1 + 1 + 2 + data_len + 2;
	uint16_t crc;

	crc16_begin(&crc, 0);
	crc16_add(&crc, &len, 1);
	crc16_add(&crc, &cmd, 1);
	crc16_add(&crc, &param1, 1);
	crc16_add(&crc, &param2, 2);
	crc16_add(&crc, data, data_len);
	crc = crc16_finish(&crc);

	uint8_t crc_buf[] = { crc, crc >> 8 };

	/*
	 * @@@ This generates an empty write to the correct I2C address.
	 * What we want is a write message with the first byte (the subaddress)
	 * zero.
	 * Nevertheless, this has the desired effect of resetting the FIFO
	 * pointers.
	 */
	// @@@ this generates garbage on I2C ... but makes things work :)
	i2c_write(atecc_i2c, atecc_addr, 0, "", 0);
	dump("L", &len, 1);
	i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, &len, 1);
	dump("C", &cmd, 1);
	i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, &cmd, 1);
	dump("P1", &param1, 1);
	i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, &param1, 1);
	dump("P2", &param2, 2);
	i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, &param2, 2);
	if (data_len) {
		dump("D", data, data_len);
		i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, data, data_len);
	}
	dump("CRC", crc_buf, 2);
	i2c_write(atecc_i2c, atecc_addr, ATECC_COMMAND, crc_buf, 2);

	return 1;
}


void atecc_wait(uint8_t cmd)
{
	unsigned ms = atecc_exec_ms[cmd];

	assert(ms);
	mdelay(ms);
}


int atecc_result(void *buf, unsigned buf_size)
{
	uint8_t len;
	uint8_t received_crc[2];
	uint16_t crc;

//printf("read(1)\n");
	i2c_read(atecc_i2c, atecc_addr, ATECC_COMMAND, &len, 1);
	dump("L", &len, 1);
	if (len < 3) {
		printf("%u-3 bytes reponse is too smal\n", len);
		// @@@ clean up
		return -1;
	}
	if (len > buf_size + 3) {
		printf("%u-2 bytes reponse too big for %u byte buffer\n",
		    len, buf_size);
		// @@@ clean up
		return -1;
	}

	uint8_t tmp[len];

//printf("read(%u)\n", len);
	i2c_read(atecc_i2c, atecc_addr, ATECC_COMMAND, tmp, len);
	dump("< ", tmp, 7);

	crc16_begin(&crc, 0);
	crc16_add(&crc, tmp, len - 2);
	received_crc[0] = tmp[len - 2];
	received_crc[1] = tmp[len - 1];
	memcpy(buf, tmp + 1, len - 3);
#if 0
printf("read(%u)\n", len - 2);
	i2c_read(atecc_i2c, atecc_addr, ATECC_COMMAND, buf, len - 2);
printf("read(2)\n");
	i2c_read(atecc_i2c, atecc_addr, ATECC_COMMAND, &received_crc, 2);

dump("< ", buf, len - 3);
dump(" C", received_crc, 2);

	crc16_begin(&crc, 0);
	crc16_add(&crc, &len, 1);
	crc16_add(&crc, buf, len - 3);
#endif

	crc = crc16_finish(&crc);
	if (crc == (received_crc[0] | received_crc[1] << 8))
		return len - 3;
	printf("CRC: received 0x%02x%02x calculated 0x%04x\n",
	    received_crc[1], received_crc[0], crc);
	return -1;
}


/* --- Commands ------------------------------------------------------------ */


void atecc_info(unsigned i2c)
{
	uint8_t revision[4];
	int got, i;


//printf("atecc_command\n");
	atecc_command(ATECC_INFO, ATECC_INFO_REVISION, 0, NULL, 0);
//printf("atecc_wait\n");
	atecc_wait(ATECC_INFO);
	got = atecc_result(revision, 4);
	if (got < 0) {
		printf("atecc_result failed\n");
		return;
	}
	printf("%d:", got);
	for (i = 0; i != got; i++)
		printf(" %02x", revision[i]);
	printf("\n");
}


/* --- Initializations ----------------------------------------------------- */


void atecc_init(unsigned i2c, unsigned addr)
{
	atecc_i2c = i2c;
	atecc_addr = addr;

}
