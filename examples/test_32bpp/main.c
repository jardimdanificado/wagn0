#define WAGN0_BPP 32
#include "wagn0.h"

static Canvas img;

void setup() {
    w_setup("32BPP Test", 320, 240, 32, 2);
    img = img_load(assets_test_card_png_data, sizeof(assets_test_card_png_data));
}

void draw() {
    clear(screen, BLACK);
    draw_text(screen, "32BPP MODE (RGBA8888)", 10, 10, WHITE);
    if (img.pixels) draw_canvas(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);
}
