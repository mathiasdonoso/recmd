CC      := gcc
VERSION := 1.0.0
CFLAGS  := -Wall -Wextra -O3 -std=c11 -D_POSIX_C_SOURCE=200809L \
		   -DRECMD_RELVER=\"$(VERSION)\"  
LDFLAGS := -lsqlite3

TARGET  := recmd
SRC     := $(wildcard *.c)
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

install: $(TARGET)
	@echo "Installing $(TARGET) to $(PREFIX)/bin..."
	@mkdir -p $(PREFIX)/bin
	@cp $(TARGET) $(PREFIX)/bin/
	@chmod 755 $(PREFIX)/bin/$(TARGET)
	@echo "Done. You can now use '$(TARGET)' anywhere."

uninstall:
	@echo "Removing $(TARGET) from $(PREFIX)/bin..."
	@rm -f $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
