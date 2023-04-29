#!/usr/bin/perl
#
# pnmtorgb.pl - Convert a PNM image to raw RGB565
#

<>;	# magic
<>;	# x y
<>;	# bpp
$s = join("", <>);

@v = unpack("C*", $s);
while ($#v > -1) {
	($r, $g, $b) = splice(@v, 0, 3);
	printf("0x%02x%02x, ",
	    (($g >> 2) & 7) << 5 | $b >> 3,
	    ($r & 0xf8) | $g >> 5);
}
