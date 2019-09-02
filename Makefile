CC = gcc

all: $(OBJECT)

SOURCE = sgpf.c
OBJECT = sgpf.o
OUTPUT = sgpf
LFLAGS = -lm -lgmp

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OBJECT)
	$(CC) -o $(OUTPUT) $^ $(LFLAGS)

clean:
	rm -f $(OUTPUT) $(OBJECT)
.PHONY: clean
