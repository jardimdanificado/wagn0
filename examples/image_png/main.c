#include "wagn0.h"

static Canvas img = {0};

void draw() {
    static int _init = 0;
    if (!_init) { _init = 1;
        img = img_load(assets_test_png_data, sizeof(assets_test_png_data));
    }

    clear(screen, BLACK);
    if (img.pixels) draw_canvas(screen, img, (320 - ASSETS_TEST_PNG_W) / 2, (240 - ASSETS_TEST_PNG_H) / 2);
}
