CROSS = riscv64-unknown-linux-gnu-

CFLAGS = -O9 -Wall -Wextra -Wshadow -Wno-unused-parameter \
	 -Wmissing-prototypes -Wmissing-declarations
COMMON_OBJS = mmio.o gpio.o
LED_OBJS = led.o $(COMMON_OBJS)
DISP_OBJS = disp.o $(COMMON_OBJS) spi.o lcd.o
TOUCH_OBJS = touch.c $(COMMON_OBJS) i2c.o cst816.o
ALL_OBJS = led.o disp.o touch.o spi.o lcd.o i2c.o $(COMMON_OBJS)

include Makefile.c-common


.PHONY:		all clean spotless

all::		led disp touch

led:		$(LED_OBJS)

disp:		$(DISP_OBJS)

disp.o:		citrine.inc

touch:		$(TOUCH_OBJS)

citrine.inc:	citrine.jpg pnmtorgb.pl
		jpegtopnm citrine.jpg | \
		    pnmscale -height 280 | pnmcut --left 75 -width 240 | \
		    ./pnmtorgb.pl >$@ || { rm -f $@; exit 1; }

IMG = https://upload.wikimedia.org/wikipedia/commons/e/e8/Citrine_1_%28Russie%29.jpg

citrine.jpg:
		wget -O $@ "$(IMG)" || { rm -f $@; exit 1; }

clean::
		rm -f $(ALL_OBJS) citrine.inc

spotless::	clean
		rm -f led
		rm -f disp
		rm -f touch
