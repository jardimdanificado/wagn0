#include "wagn0.h"
#include "posix_shim.h"
#include "gifdec.h"
#include "gif_data.h"

#define IMG_W 64
#define IMG_H 64

// gifdec outputs RGB (3 bytes/pixel). Write directly to w_vram as RGB565.
static uint8_t frame_buf[IMG_W * IMG_H * 3];

static gd_GIF* gif = 0;
static uint32_t last_frame_ticks = 0;

void setup() {
    w_setup("WagnO - GIF (gifdec)", 320, 240, 16, 2);
    _shim_reset(gif_data, gif_size);
    gif = gd_open_gif("embedded");
    if (gif) {
        gd_get_frame(gif);  // load first frame so gif->gce.delay is valid
        last_frame_ticks = w_ticks;
    }
}

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void draw() {
    background(BLACK);
    if (!gif) return;

    uint32_t now = w_ticks;
    uint32_t delay_ms = (uint32_t)gif->gce.delay * 10;
    if (delay_ms == 0) delay_ms = 100;
    while (now - last_frame_ticks >= delay_ms) {
        last_frame_ticks += delay_ms;
        if (!gd_get_frame(gif)) {
            gd_rewind(gif);
            gd_get_frame(gif);  // load first frame so the buffer updates now
        }
        delay_ms = (uint32_t)gif->gce.delay * 10;
        if (delay_ms == 0) delay_ms = 100;
    }

    gd_render_frame(gif, frame_buf);

    // Direct write: convert RGB888 → RGB565 and write to w_vram
    uint16_t* vram = (uint16_t*)w_vram;
    int ox = (320 - IMG_W) / 2;
    int oy = (240 - IMG_H) / 2;
    for (int y = 0; y < IMG_H; y++) {
        for (int x = 0; x < IMG_W; x++) {
            int i = y * IMG_W + x;
            uint16_t c = rgb565(frame_buf[i*3+0], frame_buf[i*3+1], frame_buf[i*3+2]);
            vram[(oy + y) * 320 + (ox + x)] = c;
        }
    }
}
