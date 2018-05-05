

all: font font32.c

font: font.o
	$(CC) $(CFLAGS) -o $@ $<

font.o: font.c
	$(CC) -c $(CFLAGS) -o $@ $<

font32.c: font
	./$<

clean:
	rm -f font32.c font.o font
