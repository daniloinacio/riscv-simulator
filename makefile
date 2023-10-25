IDIR =./include

CC = gcc
CFLAGS = -g -I$(IDIR)

ODIR = ./obj

SDIR = ./src

BINDIR = ./bin

_DEPS = simulator.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = simulator.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: simulator

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simulator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
