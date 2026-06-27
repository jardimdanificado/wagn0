#include "wagn0.h"

static Wagn0Image img = {0};

void setup() {
    w_setup("WagnO - PNG", 320, 240, 16, 2);
    img = png_decode(assets_test_png_data, sizeof(assets_test_png_data));
}

void draw() {
    background(BLACK);
    if (img.pixels) image(img, (320 - ASSETS_TEST_PNG_W) / 2, (240 - ASSETS_TEST_PNG_H) / 2);
}
