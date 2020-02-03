CC=gcc
CFLAGS=-g -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wunreachable-code -Wformat=2 -std=c11

ODIR=obj

DEPS=main.h display.h board.h scorecard.h difficulty.h square.h

_OBJ=main.o display.o board.o scorecard.o difficulty.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: minesweeper.out

minesweeper.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *.out
