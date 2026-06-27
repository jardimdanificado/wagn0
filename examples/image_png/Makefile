CLANG = clang
CFLAGS = --target=wasm32 -nostdlib -fno-delete-null-pointer-checks -O2 -w \
         -I../../include \
         -I../image_common/decoders \
         -I../image_common/shim/include \
         -DLODEPNG_NO_COMPILE_DISK \
         -DLODEPNG_NO_COMPILE_ENCODER \
         -DLODEPNG_NO_COMPILE_ANCILLARY_CHUNKS \
         -DLODEPNG_NO_COMPILE_ERROR_TEXT
LDFLAGS = -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined \
          -Wl,--gc-sections -Wl,--strip-debug
MEM_FLAGS = -Wl,--initial-memory=8388608

TARGET = ../../image_png.wasm
SHIM_C = ../image_common/shim/libc_shim.c
LODEPNG_C = ../image_common/decoders/lodepng.c

all: $(TARGET)

png_data.h: ../image_common/gen_image.sh
	../image_common/gen_image.sh .

$(TARGET): main.c png_data.h $(SHIM_C) $(LODEPNG_C)
	$(CLANG) $(CFLAGS) $(MEM_FLAGS) $(LDFLAGS) -o $@ main.c $(LODEPNG_C) $(SHIM_C)

clean:
	rm -f $(TARGET) png_data.h test.png

.PHONY: all clean
