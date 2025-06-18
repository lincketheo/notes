CC            := gcc
CFLAGS.base   := -I./include -Wall -Wextra -pedantic
ifeq ($(shell uname -s),Darwin)
CFLAGS.base  += -D_DARWIN_C_SOURCE
endif

SRC          := $(shell find src -type f -name '*.c')
OBJ          := $(SRC:.c=.o)

APP_SRC      := $(wildcard apps/*.c)
APP_OBJ      := $(APP_SRC:.c=.o)
APP_BIN      := $(patsubst apps/%.c,%,$(APP_SRC))

all: debug

debug: CFLAGS := $(CFLAGS.base) -O0 -g
debug: $(APP_BIN)

release: CFLAGS := $(CFLAGS.base) -O3 -DNDEBUG
release: $(APP_BIN)

%.o: %.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

apps/%.o: apps/%.c
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%: $(OBJ) apps/%.o
	@echo "  LD      $@"
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@echo "  CLEAN"
	rm -f $(OBJ) $(APP_OBJ) $(APP_BIN)

format:
	clang-format -i $(shell find src include -name '*.c' -o -name '*.h')

lint:
	clang-tidy --warnings-as-errors=* --quiet $(SRC) -- $(CFLAGS.base)

.PHONY: all debug release clean format lint
