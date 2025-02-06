# Compiler & Flags
CC := gcc
CFLAGS := -Wall -std=c11 -g

# Targets
LIBRARY := libcjson.a
BINARIES := main

.PHONY: all clean

all: $(LIBRARY) $(BINARIES)

# Build static library
$(LIBRARY): c_json.o
	ar rcs $@ $^

# Build executable and link with the static library
main: main.o $(LIBRARY)
	$(CC) -o $@ $< -L. -lcjson $(CFLAGS)

# Compile source files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean
clean:
	rm -f $(BINARIES) $(LIBRARY) *.o

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i
