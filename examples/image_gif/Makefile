CLANG = clang
CFLAGS = --target=wasm32 -nostdlib -fno-delete-null-pointer-checks -O2 -w \
         -I../../include \
         -I../image_common/decoders \
         -I../image_common/shim/include
LDFLAGS = -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined \
          -Wl,--gc-sections -Wl,--strip-debug
MEM_FLAGS = -Wl,--initial-memory=8388608

TARGET = ../../image_gif.wasm
SHIM_C = ../image_common/shim/libc_shim.c
POSIX_C = ../image_common/decoders/posix_shim.c
GIFDEC_C = ../image_common/decoders/gifdec.c

all: $(TARGET)

gif_data.h: ../image_common/gen_image.sh
	../image_common/gen_image.sh .

$(TARGET): main.c gif_data.h $(SHIM_C) $(POSIX_C) $(GIFDEC_C)
	$(CLANG) $(CFLAGS) $(MEM_FLAGS) $(LDFLAGS) -o $@ main.c $(GIFDEC_C) $(POSIX_C) $(SHIM_C)

clean:
	rm -f $(TARGET) gif_data.h test.gif

.PHONY: all clean
