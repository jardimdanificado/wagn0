#include "wagn0.h"

static Canvas img;

void preload() {
    load_image(&img, "test_card.png");
}

void draw() {
    clear(screen, BLACK);

    clear(screen, BLACK);
    draw_text(screen, "16BPP MODE (RGB565)", 10, 10, WHITE);
    if (img.pixels) draw_canvas(screen, img, (320 - img.width) / 2, (240 - img.height) / 2);
}
