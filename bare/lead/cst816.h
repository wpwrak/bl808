#ifndef CST816_H
#define	CST816_H

#include <stdbool.h>

#include "touch.h"


void cst816_read(struct touch *t);
bool cst816_poll(void);
void cst816_init(unsigned i2c, unsigned addr, unsigned int_pin);

#endif /* !CST816_H */
