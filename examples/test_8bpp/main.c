#include "wagn0.h"
#include "image.rgb332.h"

static Wagn0Image img;

void setup() {
    w_setup("8BPP Test", 320, 240, 8, 2);
    img = create_image_from_data(image_pixels, image_width, image_height, 8);
}

void draw() {
    background(BLACK);
    fill(WHITE);
    text("8BPP MODE (RGB332)", 10, 10);
    if (img.pixels) image(img, (320 - img.width) / 2, (240 - img.height) / 2);
}
