CC = gcc

all: $(OBJECT)

SOURCE = sgpf.c arithmetic.c basis.c
OBJECT = sgpf.o arithmetic.o basis.o
OUTPUT = sgpf
LFLAGS = -lm -lgmp

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OUTPUT)
	make $(OUTPUT)

sgpf: $(OBJECT)
	$(CC) -o $(OUTPUT) $^ $(LFLAGS)

sgpf.o: arithmetic.h
arithmetic.o: arithmetic.h
basis.o: arithmetic.h basis.h

clean:
	rm -f $(OUTPUT) $(OBJECT)
.PHONY: clean
