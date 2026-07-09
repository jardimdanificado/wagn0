#define WAGNER_NO_DEFAULT_CALLBACKS
#include "wagner.h"

void draw() {
    clear(screen, BLUE);
    draw_rect(screen, 10, 10, 100, 80, RED);
    draw_ellipse(screen, 200, 100, 80, 80, GREEN);
    draw_rect(screen, (int)wagn0.mouse.x - 20, (int)wagn0.mouse.y - 20, 40, 40, WHITE);
    draw_text(screen, "press SPACE to beep", 10, 220, WHITE);
}

void key_pressed(int key) {
    if (key == 0x2C) play_tone(880.0f, 0.1f, 0.3f);
}
