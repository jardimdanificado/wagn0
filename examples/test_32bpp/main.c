#include "wagn0.h"

#include "image.rgba.h"

static Olivec_Canvas _oc;
static Olivec_Canvas _img;

void setup() {
    w_setup("Wagnostic Test - 32bpp", 320, 240, 32, 2);
    _oc = olivec_canvas(w_vram, 320, 240, 320, 32);
    _img = olivec_canvas((uint32_t*)image_pixels, image_width, image_height, image_width, 32);
}

void draw() {
    olivec_fill(_oc, 0);
    int x = (320 - (int)image_width) / 2;
    int y = (240 - (int)image_height) / 2;
    olivec_sprite_copy(_oc, x, y, image_width, image_height, _img);
    olivec_text(_oc, "32BPP MODE (RGBA)", 10, 10, olivec_default_font, 2, 0xFFFFFFFF);
    
}
