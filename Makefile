CC = cc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
COMMANDS = show copy append remove count list info shell
TARGETS = $(addprefix bin/os-,$(COMMANDS))

.PHONY: all test clean
all: $(TARGETS)
bin:
	mkdir -p bin
bin/os-%: src/%.c src/io.h | bin
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LDLIBS) -o $@
test: all
	python3 tests/test_commands.py
clean:
	rm -rf bin
