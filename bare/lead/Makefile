#CC = riscv64-unknown-elf-gcc
CC = riscv64-unknown-linux-gnu-gcc

CFLAGS = -O9 -Wall -Wextra -Wshadow -Wno-unused-parameter \
	 -Wmissing-prototypes -Wmissing-declarations
OBJS = led.o mmio.o


.PHONY:		all clean spotless

all:		led

led:		$(OBJS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f led
