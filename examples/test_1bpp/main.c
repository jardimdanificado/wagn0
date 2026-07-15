#define WAGNER_CFG_BPP 1
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
    
    // Draw background stripes
    for (int i = 0; i < w_width; i += 20) {
        push();
        fill((i / 20) % 2); // Alternate 0 and 1
        translate(i, 0);
        scale(10, w_height);
        rect();
        pop();
    }
    
    // Draw a moving circle
    int px = ((int)(w_ticks / 10)) % w_width;
    push();
    fill(1); // White
    translate(px, w_height / 2);
    scale(30, 30);
    circle();
    pop();
}
