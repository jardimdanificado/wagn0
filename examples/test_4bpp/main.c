#define WAGNER_CFG_BPP 4
#define WAGNER_IMPLEMENTATION
#include "../../include/wagner.h"

void setup() {
    // Palette is now loaded automatically from wagner.json
}

void draw() {
    push();
    fill(0); // color index 0
    clear();
    pop();
    
    // Draw background color bars
    for (int i = 0; i < 16; i++) {
        push();
        fill(i);
        translate(i * 20, 0);
        scale(20, 240);
        rect();
        pop();
    }
    
    // Draw a moving rectangle
    int px = ((int)(w_ticks / 10)) % 320;
    push();
    fill(15); // White
    translate(px, 100);
    scale(40, 40);
    rect();
    pop();
    
    push();
    fill(12); // Red
    translate(px + 20, 120);
    scale(10, 10);
    circle();
    pop();
}
