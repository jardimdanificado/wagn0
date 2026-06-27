#define WAGN0_BPP 32
#include "wagn0.h"

static Wagn0Image img;

void setup() {
    w_setup("32BPP Test", 320, 240, 32, 2);
    img = png_decode(assets_test_card_png_data, sizeof(assets_test_card_png_data));
}

void draw() {
    background(BLACK);
    fill(WHITE);
    text("32BPP MODE (RGBA8888)", 10, 10);
    if (img.pixels) image(img, (320 - img.width) / 2, (240 - img.height) / 2);
}
