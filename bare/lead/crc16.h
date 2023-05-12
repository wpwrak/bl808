/*
 * crc16.c - Calculate CRC-16 with polynom x^16 + x^15 + x^2 + x^1
 */

#ifndef CRC16_H
#define	CRC16_h

#include <stdint.h>


void crc16_begin(uint16_t *crc, uint16_t initial);
void crc16_add(uint16_t *crc, const void *data, unsigned len);
uint16_t crc16_finish(uint16_t *crc);

#endif /* !CRC16_H */
