#include "wagn0.h"

static Image sheet;
static int sprite_w = 24, sprite_h = 24;

typedef struct { float x, y, vx, vy; int w, h; } Sprite;
#define SPRITE_COUNT 200
static Sprite sprites[SPRITE_COUNT];
static uint32_t last_time = 0;
static int frame_count = 0, fps = 0;
static char fps_text[16] = "fps: --";
static uint32_t seed = 12345;
static uint32_t rnd() { seed = seed * 1103515245 + 12345; return (seed / 65536) % 32768; }

void setup() {
    w_setup("WagnO - Draw", 320, 240, 16, 4);
    sheet = img_load(assets_sprite_png_data, sizeof(assets_sprite_png_data));

    for (int i = 0; i < SPRITE_COUNT; i++) {
        sprites[i].w = sprite_w; sprites[i].h = sprite_h;
        sprites[i].x = (float)(rnd() % (320 - sprite_w));
        sprites[i].y = (float)(rnd() % (240 - sprite_h));
        sprites[i].vx = (float)((rnd() % 4) + 1) * (rnd() % 2 ? -1 : 1);
        sprites[i].vy = (float)((rnd() % 4) + 1) * (rnd() % 2 ? -1 : 1);
    }
    last_time = w_ticks;
}

void draw() {
    uint32_t now = w_ticks; frame_count++;
    if (now - last_time >= 1000) {
        fps = frame_count; frame_count = 0; last_time = now;
        int n = fps;
        for (int i = 0; i < 5; i++) fps_text[i] = "fps: "[i];
        fps_text[5] = '0' + (n / 100); if (fps_text[5] == '0') fps_text[5] = ' ';
        fps_text[6] = '0' + ((n / 10) % 10); if (fps_text[5] == ' ' && fps_text[6] == '0') fps_text[6] = ' ';
        fps_text[7] = '0' + (n % 10); if (fps_text[6] == ' ') fps_text[7] = ' ';
        fps_text[8] = 0;
    }

    clear(screen, BLACK);
    if (!sheet.pixels) return;

    for (int i = 0; i < SPRITE_COUNT; i++) {
        sprites[i].x += sprites[i].vx; sprites[i].y += sprites[i].vy;
        if (sprites[i].x <= 0 || sprites[i].x + sprites[i].w >= 320) sprites[i].vx *= -1;
        if (sprites[i].y <= 0 || sprites[i].y + sprites[i].h >= 240) sprites[i].vy *= -1;
        draw_image_scaled(screen, sheet, (int)sprites[i].x, (int)sprites[i].y, sprite_w, sprite_h);
    }

}
