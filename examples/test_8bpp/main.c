#include "wagn0.h"

static Canvas img;

void draw() {
    static int _init = 0;
    if (!_init) { _init = 1;
        img = img_load(assets_test_card_png_data, sizeof(assets_test_card_png_data));
    }

    clear(screen, BLACK);
    draw_text(screen, "8BPP MODE (RGB332)", 10, 10, WHITE);
    if (img.pixels) draw_canvas(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);
}
