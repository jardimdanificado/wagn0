CLANG ?= clang
WAGN0_DIR ?= ..
INCLUDE_DIR ?= -I$(WAGN0_DIR)/include

CFLAGS = --target=wasm32 -nostdlib -fno-delete-null-pointer-checks -O2 -w $(INCLUDE_DIR)
LDFLAGS = -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined
MEM_FLAGS = -Wl,--initial-memory=8388608

TARGET ?= $(shell basename $(CURDIR)).wasm

SRCS = $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CLANG) $(CFLAGS) $(MEM_FLAGS) $(LDFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
