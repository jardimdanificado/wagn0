#include "wagn0.h"
#include "image.rgb565.h"

static Wagn0Image img;
static int pos_x = 100, pos_y = 70;
static int scale_w = 120, scale_h = 100;

void setup() {
    w_setup("WagnO - Images", 320, 240, 16, 4);
    img = create_image_from_data(image_pixels, image_width, image_height, 16);
}

void draw() {
    background(BLACK);

    if (w_gamepad_buttons & W_BTN_LEFT)  pos_x -= 2;
    if (w_gamepad_buttons & W_BTN_RIGHT) pos_x += 2;
    if (w_gamepad_buttons & W_BTN_UP)    pos_y -= 2;
    if (w_gamepad_buttons & W_BTN_DOWN)  pos_y += 2;
    if (w_gamepad_buttons & W_BTN_L1)    scale_w -= 2;
    if (w_gamepad_buttons & W_BTN_R1)    scale_w += 2;
    if (w_gamepad_buttons & W_BTN_L2)    scale_h -= 2;
    if (w_gamepad_buttons & W_BTN_R2)    scale_h += 2;
    if (w_gamepad_buttons & W_BTN_A)     { scale_w = image_width; scale_h = image_height; }

    if (img.pixels) image_scaled(img, pos_x, pos_y, scale_w, scale_h);
}
