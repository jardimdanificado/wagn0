#include "wagner.h"

static Canvas img;

void preload() {
    load_image(&img, "test_card.png");
}

void draw() {
    clear(BLACK);
    
    push();
    translate(10, 10);
    fill(WHITE);
    draw_text("32BPP MODE (RGBA8888)");
    pop();
    
    if (img.pixels) {
        push();
        translate((320 - img.width) / 2.0f, (240 - img.height) / 2.0f);
        scale(img.width, img.height);
        texture(&img);
        draw_quad();
        pop();
    }
}
