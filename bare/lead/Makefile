CROSS = riscv64-unknown-linux-gnu-

CFLAGS = -O9 -Wall -Wextra -Wshadow -Wno-unused-parameter \
	 -Wmissing-prototypes -Wmissing-declarations
COMMON_OBJS = mmio.o gpio.o
LED_OBJS = led.o $(COMMON_OBJS)
BUTTON_OBJS = button.o $(COMMON_OBJS)
DISP_OBJS = disp.o $(COMMON_OBJS) spi.o st7789.o bl.o
TOUCH_OBJS = touch.c $(COMMON_OBJS) i2c.o cst816.o
ACCEL_OBJS = accel.c $(COMMON_OBJS) i2c.o bma400.o
SE_OBJS = se.c $(COMMON_OBJS) i2c.o atecc.o crc16.o
OBJS = led.o button.o disp.o touch.o accel.o se.o \
       $(COMMON_OBJS) spi.o i2c.o bl.o \
       st7789.o cst816.o bma400.o atecc.o crc16.o

include Makefile.c-common


.PHONY:		all clean spotless

all::		led button disp touch accel se

led:		$(LED_OBJS)

button:		$(BUTTON_OBJS)

disp:		$(DISP_OBJS)

disp.o:		citrine.inc

touch:		$(TOUCH_OBJS)

accel:		$(ACCEL_OBJS)

se:		$(SE_OBJS)

citrine.inc:	citrine.jpg pnmtorgb.pl
		jpegtopnm citrine.jpg | \
		    pnmscale -height 280 | pnmcut --left 75 -width 240 | \
		    ./pnmtorgb.pl >$@ || { rm -f $@; exit 1; }

IMG = https://upload.wikimedia.org/wikipedia/commons/e/e8/Citrine_1_%28Russie%29.jpg

citrine.jpg:
		wget -O $@ "$(IMG)" || { rm -f $@; exit 1; }

clean::
		rm -f $(OBJS) citrine.inc

spotless::	clean
		rm -f led disp touch accel se
