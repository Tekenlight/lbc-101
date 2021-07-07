# Makefile for building bc library
# bc is available at http://www.gnu.org/software/bc/

CC= gcc
CFLAGS= -pedantic -Wall -Wextra -O2 $(PIC)
MYCFLAGS= $(CFLAGS) -I.
L= number.o

all:	$L

number.o: number.c number.h config.h
	$(CC) $(MYCFLAGS) -c number.c

clean:
	-rm -f *.o *.a $T
