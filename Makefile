CC = gcc

all: $(OBJECT)

SOURCE = main.c arithmetic.c basis.c slate.c gpf.c
OBJECT = main.o arithmetic.o basis.o slate.o gpf.o
WRAPPER = commandline-interface.sh
BINARY = binary
OUTPUT = sgpf
LFLAGS = -lm -lgmp

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OUTPUT)
	make $(OUTPUT)

binary: $(OBJECT)
	$(CC) -o $(BINARY) $^ $(LFLAGS)

sgpf: $(BINARY) $(WRAPPER)
	cat $(WRAPPER) > $(OUTPUT)
	chmod +x $(OUTPUT)

main.o: arithmetic.h basis.h slate.h gpf.h main.h
arithmetic.o: arithmetic.h
basis.o: arithmetic.h basis.h
slate.o: arithmetic.h basis.h slate.h
gpf.o: arithmetic.h basis.h slate.h gpf.h main.h

clean:
	rm -f $(OUTPUT) $(OBJECT) $(BINARY)
.PHONY: clean
