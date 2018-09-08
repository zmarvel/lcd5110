

LCD5110_INCDIR = include

LCD5110_CSRC = src/LCD5110.c \
       font/acorn-electron-8x8.c \
       font/atari-small.c

LCD5110_OBJS = $(patsubst %.c,%.o,$(CSRC))

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

font/acorn-electron-8x8.c: font/acorn.py
	python3 $< font/acorn-electron-8x8.bin $@

font/atari-small.c: font/atari-small.py
	python3 $< font/atari-small.bdf $@

clean:
	rm -f font/acorn-electron-8x8.c font/atari-small.c

include openocd.mk
