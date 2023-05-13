/*
 * bl.h - Backlight driver
 */

#ifndef BL_H
#define BL_H

#include <stdbool.h>


void bl_on(bool on);
void bl_init(unsigned pin);

#endif /* !BL_H */
