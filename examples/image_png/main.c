#include "wagn0.h"

static Canvas img = {0};

void setup() {
    w_setup("WagnO - PNG", 320, 240, 16, 2);
    img = img_load(assets_test_png_data, sizeof(assets_test_png_data));
}

void draw() {
    clear(screen, BLACK);
    if (img.pixels) draw_canvas(screen, img, (320 - ASSETS_TEST_PNG_W) / 2, (240 - ASSETS_TEST_PNG_H) / 2);
}
