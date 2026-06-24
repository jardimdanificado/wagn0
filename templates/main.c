#include "wagn0.h"

void setup() {
}

void update() {
}

void draw() {
    background(WAGN0_BLUE);

    fill(WAGN0_RED);
    rect(10, 10, 100, 80);

    fill(WAGN0_GREEN);
    ellipse(200, 100, 80, 80);

    stroke(WAGN0_WHITE);
    stroke_weight(3);
    no_fill();
    rect(wagn0.mouse.x - 20, wagn0.mouse.y - 20, 40, 40);
}

void mouse_pressed() {}
void mouse_released() {}
void key_pressed(int key) {}
void key_released(int key) {}
