#include "wagn0.h"
#include "image.rgb565.h"

static Wagn0Image img;

void setup() {
    w_setup("16BPP Test", 320, 240, 16, 2);
    img = create_image_from_data(image_pixels, image_width, image_height, 16);
}

void draw() {
    background(BLACK);
    fill(WHITE);
    text("16BPP MODE (RGB565)", 10, 10);
    if (img.pixels) image(img, (320 - img.width) / 2, (240 - img.height) / 2);
}
