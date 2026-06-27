#include "wagn0.h"
#include "image.rgba.h"

static Wagn0Image img;

void setup() {
    w_setup("32BPP Test", 320, 240, 32, 2);
    img = create_image_from_data(image_pixels, image_width, image_height, 32);
}

void draw() {
    background(BLACK);
    fill(WHITE);
    text("32BPP MODE (RGBA8888)", 10, 10);
    if (img.pixels) image(img, (320 - img.width) / 2, (240 - img.height) / 2);
}
