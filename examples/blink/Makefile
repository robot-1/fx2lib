FX2LIBDIR=../../
BASENAME = blink
SOURCES=blink.c
A51_SOURCES=dscr.a51
PID=0x1004

include $(FX2LIBDIR)lib/fx2.mk


test: test.cpp
	g++ -o test test.cpp -lusb-1.0

clean:
	$(RM) *.asm
