CC = gcc

all: $(OBJECT)

SOURCE = sgpf.c
OBJECT = sgpf.o
OUTPUT = sgpf
LFLAGS = -lm

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OBJECT)
	$(CC) -o $(OUTPUT) $^ $(LFLAGS)

primesum.o: $(SOURCE)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OUTPUT) $(OBJECT)
.PHONY: clean
