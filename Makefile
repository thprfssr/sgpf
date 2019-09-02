CC = gcc

all: $(OBJECT)

SOURCE = main.c arithmetic.c basis.c slate.c
OBJECT = main.o arithmetic.o basis.o slate.o
OUTPUT = sgpf
LFLAGS = -lm -lgmp

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OUTPUT)
	make $(OUTPUT)

sgpf: $(OBJECT)
	$(CC) -o $(OUTPUT) $^ $(LFLAGS)

main.o: arithmetic.h basis.h slate.h
arithmetic.o: arithmetic.h
basis.o: arithmetic.h basis.h
slate.o: arithmetic.h basis.h slate.h

clean:
	rm -f $(OUTPUT) $(OBJECT)
.PHONY: clean
