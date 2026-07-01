#include "wagn0.h"

static Canvas img;
static int pos_x = 100, pos_y = 70;
static int scale_w = 120, scale_h = 100;

void preload() {
    load_image(&img, "sprite.png");
}

void draw() {

    clear(screen, BLACK);

    if (w_gamepad_buttons & W_BTN_LEFT)  pos_x -= 2;
    if (w_gamepad_buttons & W_BTN_RIGHT) pos_x += 2;
    if (w_gamepad_buttons & W_BTN_UP)    pos_y -= 2;
    if (w_gamepad_buttons & W_BTN_DOWN)  pos_y += 2;
    if (w_gamepad_buttons & W_BTN_L1)    scale_w -= 2;
    if (w_gamepad_buttons & W_BTN_R1)    scale_w += 2;
    if (w_gamepad_buttons & W_BTN_L2)    scale_h -= 2;
    if (w_gamepad_buttons & W_BTN_R2)    scale_h += 2;
    if (w_gamepad_buttons & W_BTN_A)     { scale_w = img.width; scale_h = img.height; }

    if (img.pixels) draw_canvas_scaled(screen, img, pos_x, pos_y, scale_w, scale_h);
}
