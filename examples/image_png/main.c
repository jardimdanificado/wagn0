#include "wagn0.h"

static Canvas img = {0};

void preload() {
    load_image(&img, "test.png");
}

void draw() {
    clear(screen, BLACK);
    if (img.pixels) draw_canvas(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);
}
