#include "wagn0.h"
#include "lodepng.h"
#include "png_data.h"

static uint8_t* decoded = 0;
static unsigned img_w = 0, img_h = 0;

void setup() {
    w_setup("WagnO - PNG (lodepng)", 320, 240, 16, 2);

    unsigned error = lodepng_decode32(&decoded, &img_w, &img_h,
                                        png_data, png_size);
    if (error) {
        decoded = 0;
        return;
    }
}

void draw() {
    background(BLACK);
    if (!decoded) return;

    Wagn0Image img = create_image_from_data(decoded, img_w, img_h, 32);
    image(img, (320 - (int)img_w) / 2, (240 - (int)img_h) / 2);
}
