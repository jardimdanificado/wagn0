#define WAGNER_CFG_BPP 2
#define WAGNER_IMPLEMENTATION
#include "../../include/wagner.h"

void setup() {
    // Palette is now loaded automatically from wagner.json
}

void draw() {
    push();
    fill(0); // Black
    clear();
    pop();
    
    // Draw 4 distinct quadrants
    for (int i = 0; i < 4; i++) {
        push();
        fill(i);
        translate((i % 2) * (w_width / 2), (i / 2) * (w_height / 2));
        scale(w_width / 2, w_height / 2);
        rect();
        pop();
    }
    
    // Draw a moving rectangle that sweeps across
    int px = ((int)(w_ticks / 10)) % w_width;
    push();
    fill((px / 20) % 4); // cycles through all 4 colors
    translate(px, w_height / 2 - 20);
    scale(40, 40);
    rect();
    pop();
}
