/*
 * atecc.h - Driver for ATECC508A and similar
 */

#ifndef ATECC_H
#define	ATECC_H

#include <stdbool.h>
#include <stdint.h>


bool atecc_command(uint8_t cmd, uint8_t param1, uint16_t param2,
    const void *data, unsigned data_len);
void atecc_wait(uint8_t cmd);
int atecc_result(void *buf, unsigned buf_size);

void atecc_info(unsigned i2c);

void atecc_init(unsigned i2c, unsigned addr);

#endif /* !ATECC_H */
