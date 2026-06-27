#include "wagn0.h"

// gifdec renders RGB (3 bytes/pixel) into frame_buf. Expand to RGBA
// since wagn0's image() supports bpp 8/16/32 but not bpp 24.
#define IMG_W 64
#define IMG_H 64

static uint8_t frame_buf[IMG_W * IMG_H * 3];
static uint8_t rgba_buf[IMG_W * IMG_H * 4];
static gd_GIF* gif = 0;
static uint32_t last_ticks = 0;

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

void draw() {
    static int _init = 0;
    if (!_init) { _init = 1;
        _shim_reset(assets_test_gif_data, sizeof(assets_test_gif_data));
        gif = gd_open_gif("embedded");
        if (gif) gd_get_frame(gif);
        last_ticks = w_ticks;
    }

    clear(screen, BLACK);
    if (!gif) return;

    uint32_t now = w_ticks;
    uint32_t delay_ms = (uint32_t)gif->gce.delay * 10;
    if (delay_ms == 0) delay_ms = 100;
    while (now - last_ticks >= delay_ms) {
        last_ticks += delay_ms;
        if (!gd_get_frame(gif)) { gd_rewind(gif); gd_get_frame(gif); }
        delay_ms = (uint32_t)gif->gce.delay * 10;
        if (delay_ms == 0) delay_ms = 100;
    }

    gd_render_frame(gif, frame_buf);

    // RGB → RGBA → w_vram as RGB565
    uint16_t* vram = (uint16_t*)w_vram;
    int ox = (320 - IMG_W) / 2, oy = (240 - IMG_H) / 2;
    for (int y = 0; y < IMG_H; y++) {
        for (int x = 0; x < IMG_W; x++) {
            int i = y * IMG_W + x;
            vram[(oy + y) * 320 + (ox + x)] = rgb565(
                frame_buf[i*3+0], frame_buf[i*3+1], frame_buf[i*3+2]);
        }
    }
}
