#include "wagn0.h"

static Image img;

void setup() {
    w_setup("16BPP Test", 320, 240, 16, 2);
    img = img_load(assets_test_card_png_data, sizeof(assets_test_card_png_data));
}

void draw() {
    clear(screen, BLACK);
    draw_text(screen, "16BPP MODE (RGB565)", 10, 10, WHITE);
    if (img.pixels) draw_image(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);
}
