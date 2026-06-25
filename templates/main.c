#include "wagn0.h"

void setup() {
    w_setup("My Game", 320, 240, 16, 2);
}

void draw() {
    background(BLUE);
    fill(RED);
    rect(10, 10, 100, 80);
    fill(GREEN);
    ellipse(200, 100, 80, 80);
    stroke(WHITE);
    stroke_weight(3);
    no_fill();
    rect(wagn0.mouse.x - 20, wagn0.mouse.y - 20, 40, 40);
    fill(WHITE);
    text("press space to beep", 10, 220);
}

void key_pressed(int key) {
    if (key == 0x2C) play_tone(880.0f, 0.1f, 0.3f);  // Space
}
